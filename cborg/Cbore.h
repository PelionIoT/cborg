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
#include <cstddef>

#include "cborg/CborgHeader.h"


class Cbore
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
    } major_type_t;

    typedef enum {
        TypeFalse       = 0x14,
        TypeTrue        = 0x15,
        TypeNull        = 0x16,
        TypeUndefined   = 0x17,
        TypeUnknown     = 0x18,
        TypeHalfFloat   = 0x19,
        TypeSingleFloat = 0x1A,
        TypeDoubleFloat = 0x1B
    } cbor_simple_t;

    Cbore();

    Cbore(uint8_t* cbor, std::size_t maxLength);

    std::size_t getLength() const;

    /* Encode methods */
    Cbore& tag(uint32_t tag);
    Cbore& array(std::size_t items);
    Cbore& map(std::size_t items);

    template <std::size_t I>
    Cbore& write(const char (&string)[I])
    {
        writeTypeAndValue(TypeString, I - 1);
        writeBytes((const uint8_t*) string, I - 1);

        return *this;
    }

    /* map insertion */
    template <std::size_t I, std::size_t J>
    Cbore& item(const char (&key)[I], const char (&value)[J])
    {
        if ((itemSize(I) + itemSize(J) + I + J) <= (maxLength - currentLength))
        {
            writeTypeAndValue(TypeString, I - 1);
            writeBytes((const uint8_t*) key, I - 1);
            writeTypeAndValue(TypeString, J - 1);
            writeBytes((const uint8_t*) value, J - 1);
        }

        return *this;
    }

    template <std::size_t I>
    Cbore& item(const char (&key)[I], int32_t value)
    {
        if ((itemSize(I) + itemSize(value) + I) <= (maxLength - currentLength))
        {
            writeTypeAndValue(TypeString, I - 1);
            writeBytes((const uint8_t*) key, I - 1);

            if (value < 0)
            {
                writeTypeAndValue(TypeNegative, -1 - value);
            }
            else
            {
                writeTypeAndValue(TypeUnsigned, value);
            }
        }

        return *this;
    }

    template <std::size_t I>
    Cbore& item(int32_t key, const char (&value)[I])
    {
        if ((itemSize(I) + itemSize(key) + I) <= (maxLength - currentLength))
        {
            if (key < 0)
            {
                writeTypeAndValue(TypeNegative, -1 - key);
            }
            else
            {
                writeTypeAndValue(TypeUnsigned, key);
            }

            writeTypeAndValue(TypeString, I - 1);
            writeBytes((const uint8_t*) value, I - 1);
        }

        return *this;
    }

    Cbore item(int32_t key, int32_t value)
    {
        if ((itemSize(key) + itemSize(value)) <= (maxLength - currentLength))
        {
            if (key < 0)
            {
                writeTypeAndValue(TypeNegative, -1 - key);
            }
            else
            {
                writeTypeAndValue(TypeUnsigned, key);
            }

            if (value < 0)
            {
                writeTypeAndValue(TypeNegative, -1 - value);
            }
            else
            {
                writeTypeAndValue(TypeUnsigned, value);
            }
        }

        return *this;
    }


    /* debug */
    void print();

private:
    uint8_t itemSize(int32_t item);
    uint8_t writeTypeAndValue(major_type_t majorType, uint32_t value);
    uint32_t writeBytes(const uint8_t* source, uint32_t length);

private:
    uint8_t* cbor;
    std::size_t currentLength;
    std::size_t maxLength;
};

#endif // __CBORE_H__
