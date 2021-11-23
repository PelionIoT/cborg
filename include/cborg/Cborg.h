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

#ifndef __CBORG_H__
#define __CBORG_H__

#include <stdint.h>

#include <cstddef>
#include <string>

#include "cborg/CborBase.h"
#include "cborg/CborgHeader.h"

class Cborg {
 public:
  Cborg();
  Cborg(const uint8_t* cbor, std::size_t maxLength);
  Cborg(const uint8_t* cbor, std::size_t maxLength, std::int32_t _arrayUnits);

  bool isNull();

  /* Decode methods */
  bool getCBOR(const uint8_t** pointer, uint32_t* length);
  uint32_t getCBORLength();

  /* map functions */
  template <std::size_t I>
  [[nodiscard]] Cborg find(const char (&key)[I]) {
    return find(key, I - 1);
  }

  [[nodiscard]] Cborg find(int32_t key) const;
  [[nodiscard]] Cborg find(const char* key, std::size_t keyLength) const;

  [[nodiscard]] Cborg getKey();
  bool getKey(std::string& str);
  bool getKey(int32_t& str);
  bool getKey(uint32_t& str);

  [[nodiscard]] Cborg getValue();
  bool getValueUnsigned(uint32_t& value);
  bool getValueNegative(int32_t& value);
  bool getValueSigned(int32_t& value);

  bool getValueTimeStamp(time_t& value);

  bool getValueBytes(const uint8_t** pointer, uint32_t* length);
  bool getValueString(const char** pointer, uint32_t* length);
  bool getValueString(std::string& str);

  [[nodiscard]] Cborg at(std::size_t index) const;
  [[nodiscard]] Cborg nextArrayItem(std::size_t index = 1) const;
  [[nodiscard]] Cborg nextMapItem(std::size_t index = 1) const;

  [[nodiscard]] uint32_t getSize() const;

  /* non-container functions */
  bool getUnsigned(uint32_t&) const;
  bool getNegative(int32_t&) const;
  bool getSigned(int32_t&) const;

  bool getTimeStamp(time_t&) const;

  bool getBytes(const uint8_t** pointer, uint32_t* length) const;
  bool getString(const char** pointer, uint32_t* length) const;
  bool getString(std::string& str) const;

  /* pass through to header */
  uint32_t getTag() const;
  uint8_t getType() const;
  uint8_t getMinorType() const;

  /* debug */
  void print() const;

 private:
  const uint8_t* cbor;
  std::size_t maxLength;
  std::int32_t _arrayUnits = std::numeric_limits<std::int32_t>::max();
};

#endif  // __CBORG_H__
