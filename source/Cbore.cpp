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

#include "cborg/Cbore.h"

#include <stdio.h>
#include <string.h>

#include <list>

#include "cborg/Cborg.h"

#if 0
#include <stdio.h>
#define DEBUG_PRINTF(...) \
  { printf(__VA_ARGS__); }
#else
#define DEBUG_PRINTF(...)
#endif

Cbore::Cbore() : cbor(nullptr), currentLength(0), maxLength(0) {}

Cbore::Cbore(uint8_t* _cbor, std::size_t _length)
    : cbor(_cbor), currentLength(0), maxLength(_length) {}

std::size_t Cbore::getLength() const { return currentLength; }

/*****************************************************************************/
/* Encoding                                                                  */
/*****************************************************************************/

Cbore& Cbore::tag(uint32_t tag) {
  if ((cbor != nullptr) && (itemSize(tag) <= (maxLength - currentLength))) {
    writeTypeAndValue(CborBase::TypeTag, tag);
  }

  return *this;
}

Cbore& Cbore::end() {
  if ((cbor != nullptr) && (currentLength < maxLength)) {
    cbor[currentLength++] =
        CborBase::TypeSpecial << 5 | CborBase::TypeIndefinite;
  }

  return *this;
}

/*************************************************************************/
/* Array creation                                                        */
/*************************************************************************/

// create indefinite array
Cbore& Cbore::array() {
  if ((cbor != nullptr) && (currentLength < maxLength)) {
    cbor[currentLength++] = CborBase::TypeArray << 5 | CborBase::TypeIndefinite;
  }

  return *this;
}

// create arrray in array
Cbore& Cbore::array(std::size_t items) {
  if ((cbor != nullptr) && (itemSize(static_cast<std::uint32_t>(items)) <=
                            (maxLength - currentLength))) {
    writeTypeAndValue(CborBase::TypeArray, items);
  }

  return *this;
}

/*************************************************************************/
/* Array insertion                                                       */
/*************************************************************************/

// insert integer
Cbore& Cbore::item(int32_t value) {
  if (itemSize(value) <= (maxLength - currentLength)) {
    if (value < 0) {
      writeTypeAndValue(CborBase::TypeNegative, -1 - value);
    } else {
      writeTypeAndValue(CborBase::TypeUnsigned, value);
    }
  }

  return *this;
}

Cbore& Cbore::item(uint32_t value) {
  if (itemSize(value) <= (maxLength - currentLength)) {
    writeTypeAndValue(CborBase::TypeUnsigned, value);
  }

  return *this;
}

Cbore& Cbore::item(bool value) {
  if (value) {
    return this->item(CborBase::TypeTrue);
  }
  return this->item(CborBase::TypeFalse);
}

Cbore& Cbore::item(std::chrono::system_clock::time_point unixTimeStamp) {
  time_t time_stamp = std::chrono::system_clock::to_time_t(unixTimeStamp);
  if (itemSize(static_cast<std::uint32_t>(time_stamp)) <=
      (maxLength - currentLength)) {
    writeTypeAndValue(CborBase::TypeTag, 1);
    writeTypeAndValue(CborBase::TypeUnsigned, time_stamp);
  }

  return *this;
}

// insert simple type
Cbore& Cbore::item(CborBase::SimpleType_t simpleType) {
  if (currentLength < maxLength) {
    cbor[currentLength++] = CborBase::TypeSpecial << 5 | simpleType;
  }

  return *this;
}

Cbore& Cbore::item(const uint8_t* bytes, std::size_t length) {
  if ((itemSize(static_cast<std::uint32_t>(length)) + length) <=
      (maxLength - currentLength)) {
    writeTypeAndValue(CborBase::TypeBytes, length);
    writeBytes(bytes, length);
  }

  return *this;
}

// write string, length
Cbore& Cbore::item(const char* string, std::size_t length) {
  if ((itemSize(static_cast<std::uint32_t>(length)) + length) <=
      (maxLength - currentLength)) {
    writeTypeAndValue(CborBase::TypeString, length);
    writeBytes(reinterpret_cast<const uint8_t*>(string), length);
  }

  return *this;
}

/*************************************************************************/
/* Map creation                                                          */
/*************************************************************************/

// create indefinite map
Cbore& Cbore::map() {
  if ((cbor != nullptr) && (currentLength < maxLength)) {
    cbor[currentLength++] = CborBase::TypeMap << 5 | CborBase::TypeIndefinite;
  }

  return *this;
}

// create map in array
Cbore& Cbore::map(std::size_t items) {
  if ((cbor != nullptr) && (itemSize(static_cast<std::uint32_t>(items)) <=
                            (maxLength - currentLength))) {
    writeTypeAndValue(CborBase::TypeMap, items);
  }

  return *this;
}

/*************************************************************************/
/* Map insertion - key                                                   */
/*************************************************************************/

// insert key as integer
Cbore& Cbore::key(int32_t unit) {
  if ((itemSize(unit)) <= (maxLength - currentLength)) {
    if (unit < 0) {
      writeTypeAndValue(CborBase::TypeNegative, -1 - unit);
    } else {
      writeTypeAndValue(CborBase::TypeUnsigned, unit);
    }
  }

  return *this;
}

Cbore& Cbore::key(uint32_t unit) {
  if ((itemSize(unit)) <= (maxLength - currentLength)) {
    writeTypeAndValue(CborBase::TypeUnsigned, unit);
  }

  return *this;
}

// insert key as const char pointer with length
Cbore& Cbore::key(const char* unit, std::size_t length) {
  if ((itemSize(static_cast<std::uint32_t>(length)) + length) <=
      (maxLength - currentLength)) {
    writeTypeAndValue(CborBase::TypeString, length);
    writeBytes(reinterpret_cast<const uint8_t*>(unit), length);
  }

  return *this;
}

Cbore& Cbore::key(std::string_view str) {
  if ((itemSize(str)) <= (maxLength - currentLength)) {
    writeTypeAndValue(CborBase::TypeString, str.size());
    writeBytes(reinterpret_cast<const uint8_t*>(str.data()), str.size());
  }

  return *this;
}

/*************************************************************************/
/* Map insertion - value                                                 */
/*************************************************************************/

Cbore& Cbore::value(int32_t unit) {
  if ((itemSize(unit)) <= (maxLength - currentLength)) {
    if (unit < 0) {
      writeTypeAndValue(CborBase::TypeNegative, -1 - unit);
    } else {
      writeTypeAndValue(CborBase::TypeUnsigned, unit);
    }
  }

  return *this;
}

Cbore& Cbore::value(uint32_t unit) {
  if ((itemSize(unit)) <= (maxLength - currentLength)) {
    writeTypeAndValue(CborBase::TypeUnsigned, unit);
  }

  return *this;
}

Cbore& Cbore::value(CborBase::SimpleType_t unit) {
  if (currentLength < maxLength) {
    cbor[currentLength++] = CborBase::TypeSpecial << 5 | unit;
  }

  return *this;
}

Cbore& Cbore::value(bool unit) {
  if (unit) {
    return value(CborBase::TypeTrue);
  }
  return value(CborBase::TypeFalse);
}

Cbore& Cbore::value(const uint8_t* unit, std::size_t length) {
  if ((itemSize(static_cast<std::uint32_t>(length)) + length) <=
      (maxLength - currentLength)) {
    writeTypeAndValue(CborBase::TypeBytes, length);
    writeBytes(unit, length);
  }

  return *this;
}

Cbore& Cbore::value(const char* unit, std::size_t length) {
  if ((itemSize(static_cast<std::uint32_t>(length)) + length) <=
      (maxLength - currentLength)) {
    writeTypeAndValue(CborBase::TypeString, length);
    writeBytes(reinterpret_cast<const uint8_t*>(unit), length);
  }

  return *this;
}

Cbore& Cbore::value(std::string_view unit) {
  if ((itemSize(unit.size())) <=
      (maxLength - currentLength)) {
    writeTypeAndValue(CborBase::TypeString, unit.size());
    writeBytes(reinterpret_cast<const uint8_t*>(unit.data()), unit.size());
  }

  return *this;
}

Cbore& Cbore::reset(bool resetBuffer) {
  currentLength = 0;
  if (resetBuffer) {
    memset(cbor, 0x00, maxLength);
  }

  return *this;
}

/*****************************************************************************/
/* Helper Functions                                                          */
/*****************************************************************************/

uint8_t Cbore::itemSize(int32_t item) {
  item = (item < 0) ? -1 - item : item;

  return (itemSize(static_cast<std::uint32_t>(item)));
}

uint8_t Cbore::itemSize(uint32_t item) {
  if (item <= 23) {
    return 1;
  }
  if (item <= 0xFF) {
    return 2;
  }
  if (item <= 0xFFFF) {
    return 3;
  }
  return 5;
}

uint8_t Cbore::itemSize(std::size_t item) {
  return itemSize(static_cast<uint32_t>(item));
}

size_t Cbore::itemSize(std::string_view str) {
  return (itemSize(str.size()) + str.size());
}

uint8_t Cbore::writeTypeAndValue(CborBase::MajorType_t majorType,
                                 uint32_t value) {
  if (cbor != nullptr) {
    if (majorType < CborBase::TypeSpecial) {
      uint8_t major_type_high = majorType << 5;

      uint32_t remaining_length = maxLength - currentLength;

      // value fits in one byte
      if (value <= 23) {
        if (remaining_length >= 1) {
          cbor[currentLength++] = major_type_high | value;

          return 1;
        }
      }
      // value fits in two bytes
      else if (value <= 0xFF) {
        if (remaining_length >= 2) {
          cbor[currentLength++] = major_type_high | 24;
          cbor[currentLength++] = value;

          return 2;
        }
      }
      // value fits in three bytes
      else if (value <= 0xFFFF) {
        if (remaining_length >= 3) {
          cbor[currentLength++] = major_type_high | 25;
          cbor[currentLength++] = value >> 8;
          cbor[currentLength++] = value;

          return 3;
        }
      }
      // value fits in four bytes
      else {
        if (remaining_length >= 5) {
          cbor[currentLength++] = major_type_high | 26;
          cbor[currentLength++] = value >> 24;
          cbor[currentLength++] = value >> 16;
          cbor[currentLength++] = value >> 8;
          cbor[currentLength++] = value;

          return 5;
        }
      }
    }
  }

  return 0;
}

uint32_t Cbore::writeBytes(const uint8_t* source, uint32_t length) {
  if ((cbor != nullptr) && (source != nullptr) &&
      (length <= (maxLength - currentLength))) {
    memcpy(&cbor[currentLength], source, length);
    currentLength += length;

    return length;
  }

  return 0;
}

/*****************************************************************************/
/* Debug related                                                             */
/*****************************************************************************/

void Cbore::print() const {
  Cborg decoder(cbor, maxLength);
  decoder.print();
}
uint8_t Cbore::itemSizeBool() { return 1; }
