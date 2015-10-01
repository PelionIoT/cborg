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

#include "cborg/CborHeader.h"


class Cborg
{
public:
    typedef enum {
        TypeUnsigned    = 0x00,
        TypeNegative    = 0x01,
        TypeBytes       = 0x02,
        TypeString      = 0x03,
        TypeArray       = 0x04,
        TypeMap         = 0x05,
        TypeTag         = 0x06,
        TypeSpecial     = 0x07,
        TypeUnassigned  = 0xFF
    } cbor_type_t;

    typedef enum {
        TypeTrue        = 0x14,
        TypeFalse       = 0x15,
        TypeNull        = 0x16,
        TypeUndefined   = 0x17,
        TypeUnknown     = 0x18,
        TypeHalfFloat   = 0x19,
        TypeSingleFloat = 0x1A,
        TypeDoubleFloat = 0x1B
    } cbor_simple_t;

    Cborg();

    Cborg(const uint8_t* cbor, std::size_t cborLength);

    bool getCBOR(const uint8_t** pointer, uint32_t* length);

    /* map functions */
    template <std::size_t I>
    Cborg find(const char (&key)[I])
    {
        return find(key, I - 1);
    }

    Cborg find(const char* key, std::size_t keyLength);

    Cborg at(std::size_t index);

    uint32_t getSize() const;

    /* non-container functions */
    bool getUnsigned(uint32_t*) const;
    bool getNegative(int32_t*) const;

    bool getBytes(const uint8_t** pointer, uint32_t* length) const;
    bool getString(const char** pointer, uint32_t* length) const;

    /* pass through to header */
    uint32_t getTag() const;
    uint8_t getType() const;
    uint8_t getMinorType() const;

    /* debug */
    void print();


    bool equals(const uint8_t* candidate);





private:
    const uint8_t* cbor;
    std::size_t cborLength;
};

#endif // __CBORG_H__
