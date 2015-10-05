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
        writeTypeAndValue(TypeTag, tag);
    }

    return *this;
}

Cbore& Cbore::array(std::size_t items)
{
    if ((cbor) && (itemSize(items) <= (maxLength - currentLength)))
    {
        writeTypeAndValue(TypeArray, items);
    }

    return *this;
}

Cbore& Cbore::map(std::size_t items)
{
    if ((cbor) && (itemSize(items) <= (maxLength - currentLength)))
    {
        writeTypeAndValue(TypeMap, items);
    }

    return *this;
}


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

uint8_t Cbore::writeTypeAndValue(major_type_t majorType, uint32_t value)
{
    if (cbor)
    {
        if (majorType < TypeSpecial)
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
    printf("write: %p %p %u %u %u\r\n", cbor, source, length, currentLength, maxLength);

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

}
