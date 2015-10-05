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
#include "cborg/Cborg.h"

#include <list>

#if 0
#include <stdio.h>
#define DEBUG_PRINTF(...) { printf(__VA_ARGS__); }
#else
#define DEBUG_PRINTF(...)
#endif


Cbore::Cbore()
    :   cbor(NULL),
        currentLength(0),
        maxLength(0)
{}

Cbore::Cbore(uint8_t* _cbor, std::size_t _length)
    :   cbor(_cbor),
        currentLength(0),
        maxLength(_length)
{}

std::size_t Cbore::getLength() const
{
    return currentLength;
}

/*****************************************************************************/
/* Encoding                                                                  */
/*****************************************************************************/

Cbore& Cbore::tag(uint32_t tag)
{
    if ((cbor) && (itemSize(tag) <= (maxLength - currentLength)))
    {
        writeTypeAndValue(CborBase::TypeTag, tag);
    }

    return *this;
}

/*************************************************************************/
/* Array creation                                                        */
/*************************************************************************/

// create arrray in array
Cbore& Cbore::array(std::size_t items)
{
    if ((cbor) && (itemSize(items) <= (maxLength - currentLength)))
    {
        writeTypeAndValue(CborBase::TypeArray, items);
    }

    return *this;
}

// create array in map <integer, array>
Cbore& Cbore::array(int32_t key, std::size_t size)
{
    if ((itemSize(key) + itemSize(size)) <= (maxLength - currentLength))
    {
        if (key < 0)
        {
            writeTypeAndValue(CborBase::TypeNegative, -1 - key);
        }
        else
        {
            writeTypeAndValue(CborBase::TypeUnsigned, key);
        }

        writeTypeAndValue(CborBase::TypeArray, size);
    }

    return *this;
}

/*************************************************************************/
/* Array insertion                                                       */
/*************************************************************************/

// insert integer
Cbore& Cbore::item(int32_t value)
{
    if (itemSize(value) <= (maxLength - currentLength))
    {
        if (value < 0)
        {
            writeTypeAndValue(CborBase::TypeNegative, -1 - value);
        }
        else
        {
            writeTypeAndValue(CborBase::TypeUnsigned, value);
        }
    }

    return *this;
}

// insert simple type
Cbore& Cbore::item(CborBase::SimpleType_t simpleType)
{
    if (currentLength < maxLength)
    {
        cbor[currentLength++] = CborBase::TypeSpecial << 5 | simpleType;
    }

    return *this;
}

/*************************************************************************/
/* Map creation                                                          */
/*************************************************************************/

// create map in array
Cbore& Cbore::map(std::size_t items)
{
    if ((cbor) && (itemSize(items) <= (maxLength - currentLength)))
    {
        writeTypeAndValue(CborBase::TypeMap, items);
    }

    return *this;
}

// create map in map <integer, map>
Cbore& Cbore::map(int32_t key, std::size_t size)
{
    if ((itemSize(key) + itemSize(size)) <= (maxLength - currentLength))
    {
        if (key < 0)
        {
            writeTypeAndValue(CborBase::TypeNegative, -1 - key);
        }
        else
        {
            writeTypeAndValue(CborBase::TypeUnsigned, key);
        }

        writeTypeAndValue(CborBase::TypeMap, size);
    }

    return *this;
}

/*************************************************************************/
/* Map insertion                                                         */
/*************************************************************************/

// insert <integer, integer>
Cbore& Cbore::item(int32_t key, int32_t value)
{
    if ((itemSize(key) + itemSize(value)) <= (maxLength - currentLength))
    {
        if (key < 0)
        {
            writeTypeAndValue(CborBase::TypeNegative, -1 - key);
        }
        else
        {
            writeTypeAndValue(CborBase::TypeUnsigned, key);
        }

        if (value < 0)
        {
            writeTypeAndValue(CborBase::TypeNegative, -1 - value);
        }
        else
        {
            writeTypeAndValue(CborBase::TypeUnsigned, value);
        }
    }

    return *this;
}

// insert <integer, simple type>
Cbore& Cbore::item(int32_t key, CborBase::SimpleType_t value)
{
    if (itemSize(key) + 1 <= (maxLength - currentLength))
    {
        if (key < 0)
        {
            writeTypeAndValue(CborBase::TypeNegative, -1 - key);
        }
        else
        {
            writeTypeAndValue(CborBase::TypeUnsigned, key);
        }

        cbor[currentLength++] = CborBase::TypeSpecial << 5 | value;
    }

    return *this;
}

/*****************************************************************************/
/* Helper Functions                                                          */
/*****************************************************************************/

uint8_t Cbore::itemSize(int32_t item)
{
    item = (item < 0) ? -1 - item : item;

    if (item <= 23)
    {
        return 1;
    }
    else if (item <= 0xFF)
    {
        return 2;
    }
    else if (item <= 0xFFFF)
    {
        return 3;
    }
    else
    {
        return 5;
    }
}

uint8_t Cbore::writeTypeAndValue(CborBase::MajorType_t majorType, uint32_t value)
{
    if (cbor)
    {
        if (majorType < CborBase::TypeSpecial)
        {
            uint8_t majorTypeHigh = majorType << 5;

            uint32_t remainingLength = maxLength - currentLength;

            // value fits in one byte
            if (value <= 23)
            {
                if (remainingLength >= 1)
                {
                    cbor[currentLength++] = majorTypeHigh | value;

                    return 1;
                }
            }
            // value fits in two bytes
            else if (value <= 0xFF)
            {
                if (remainingLength >= 2)
                {
                    cbor[currentLength++] = majorTypeHigh | 24;
                    cbor[currentLength++] = value;

                    return 2;
                }
            }
            // value fits in three bytes
            else if (value <= 0xFFFF)
            {
                if (remainingLength >= 3)
                {
                    cbor[currentLength++] = majorTypeHigh | 25;
                    cbor[currentLength++] = value >> 8;
                    cbor[currentLength++] = value;

                    return 3;
                }
            }
            // value fits in four bytes
            else
            {
                if (remainingLength >= 5)
                {
                    cbor[currentLength++] = majorTypeHigh | 26;
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

uint32_t Cbore::writeBytes(const uint8_t* source, uint32_t length)
{
    if ((cbor) && (source) && (length <= (maxLength - currentLength)))
    {
        memcpy(&cbor[currentLength], source, length);
        currentLength += length;

        return length;
    }

    return 0;
}

/*****************************************************************************/
/* Debug related                                                             */
/*****************************************************************************/

void Cbore::print()
{
    Cborg decoder(cbor, maxLength);
    decoder.print();
}
