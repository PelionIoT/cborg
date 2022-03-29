/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CBORE_H__
#define __CBORE_H__

#include <stdint.h>

#include <chrono>
#include <cstddef>
#include <string_view>

#include "cborg/CborBase.h"
#include "cborg/CborgHeader.h"

class Cbore {
 public:
  Cbore();

  Cbore(uint8_t* cbor, std::size_t maxLength);

  std::size_t getLength() const;

  /* Encode methods */
  Cbore& tag(uint32_t tag);

  // end indefinite map/array
  Cbore& end();

  /*************************************************************************/
  /* Array creation                                                        */
  /*************************************************************************/

  // create indefinite map
  Cbore& array();

  // create array with size
  Cbore& array(std::size_t items);

  /*************************************************************************/
  /* Array insertion                                                       */
  /*************************************************************************/

  // write <integer>
  Cbore& item(int32_t value);
  Cbore& item(uint32_t value);

  // write <bool>
  Cbore& item(bool value);

  // write <timestamp>
  Cbore& item(std::chrono::system_clock::time_point unixTimeStamp);

  // write <simple type>
  Cbore& item(CborBase::SimpleType_t simpleType);

  // write <string>
  template <std::size_t I>
  Cbore& item(const char (&string)[I]) {
    if ((itemSize(static_cast<std::uint32_t>(I)) + I) <=
        (maxLength - currentLength)) {
      writeTypeAndValue(CborBase::TypeString, I - 1);
      writeBytes(reinterpret_cast<const uint8_t*>(string), I - 1);
    }

    return *this;
  }

  // write bytes, length
  Cbore& item(const uint8_t* bytes, std::size_t length);

  // write string, length
  Cbore& item(const char* string, std::size_t length);

  /*************************************************************************/
  /* Map creation                                                          */
  /*************************************************************************/

  // create indefinite map
  Cbore& map();

  // create map with size
  Cbore& map(std::size_t items);

  /*************************************************************************/
  /* Map insertion - key                                                   */
  /*************************************************************************/

  // insert key as integer
  Cbore& key(int32_t unit);
  Cbore& key(uint32_t unit);

  // insert key as const char array
  template <std::size_t I>
  Cbore& key(const char (&unit)[I]) {
    if ((itemSize(static_cast<std::uint32_t>(I)) + I) <=
        (maxLength - currentLength)) {
      writeTypeAndValue(CborBase::TypeString, I - 1);
      writeBytes(reinterpret_cast<const uint8_t*>(unit), I - 1);
    }

    return *this;
  }

  // insert key as const char pointer with length
  Cbore& key(const char* unit, std::size_t length);

  // insert key as string view
  Cbore& key(std::string_view str);

  /*************************************************************************/
  /* Map insertion - value                                                 */
  /*************************************************************************/

  // insert value as integer
  Cbore& value(int32_t unit);
  Cbore& value(uint32_t unit);

  // insert value as simple type
  Cbore& value(CborBase::SimpleType_t value);

  // insert value as simple bool
  Cbore& value(bool value);

  // insert value as const char array
  template <std::size_t I>
  Cbore& value(const char (&unit)[I]) {
    if ((itemSize(static_cast<std::uint32_t>(I)) + I) <=
        (maxLength - currentLength)) {
      writeTypeAndValue(CborBase::TypeString, I - 1);
      writeBytes(reinterpret_cast<const std::uint8_t*>(unit), I - 1);
    }

    return *this;
  }

  // insert value as byte array with length
  Cbore& value(const uint8_t* unit, std::size_t length);

  // insert value as const char pointer with length
  Cbore& value(const char* unit, std::size_t length);

  // insert value as string view
  Cbore& value(std::string_view unit);

  // get the size of an item (used to calculate buffer size)
  static std::uint8_t itemSize(int32_t item);
  static std::uint8_t itemSize(uint32_t item);

  template <typename T = std::enable_if<
                !std::is_same_v<std::size_t, std::uint32_t>>>  // NOLINT
  static std::uint8_t itemSize(std::size_t item) {
    return itemSize(static_cast<uint32_t>(item));
  };

  static std::uint8_t itemSizeBool();
  static size_t itemSize(std::string_view str);

  /*************************************************************************/
  /* Reset                                                                 */
  /*************************************************************************/
  Cbore& reset(bool resetBuffer);

  /*************************************************************************/
  /* Debug                                                                 */
  /*************************************************************************/

  /* debug */
  void print() const;

 private:
  uint8_t writeTypeAndValue(CborBase::MajorType_t majorType, uint32_t value);
  uint32_t writeBytes(const uint8_t* source, uint32_t length);

  uint8_t* cbor;
  std::size_t currentLength;
  std::size_t maxLength;
};

#endif  // __CBORE_H__
