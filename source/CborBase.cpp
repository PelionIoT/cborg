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

#include "cborg/CborBase.h"

#include <stdio.h>
#include <list>

CborBase CborNull;

uint32_t CborBase::writeCBOR(uint8_t* destination, uint32_t maxLength)
{
    uint32_t written = 0;

    if (destination)
    {
        // write tag if set
        if (tag != TypeUnassigned)
        {
            written += writeTypeAndValue(destination, maxLength, TypeTag, tag);
        }

        if (written + 1 <= maxLength)
        {
            destination[written] = majorType << 5 | minorType;
            written++;
        }
    }

    return written;
}

uint32_t CborBase::writeQueue(uint8_t* destination, uint32_t maxLength, std::list<CborBase*>& queue)
{
    uint32_t written = 0;

    if (destination)
    {
        while (queue.size() > 0)
        {
            CborBase* currentObject = queue.back();
            queue.pop_back();

            if (currentObject)
            {
                uint8_t type = currentObject->getType();

                if (type == CborBase::TypeArray)
                {
                    // write tag if set
                    uint32_t subtag = currentObject->getTag();
                    if (subtag != TypeUnassigned)
                    {
                        written += writeTypeAndValue(&destination[written], maxLength - written, TypeTag, subtag);
                    }

                    uint32_t items = currentObject->getSize();
                    written += CborBase::writeTypeAndValue(&destination[written], maxLength - written, CborBase::TypeArray, items);

                    // dump array to FILO queue
                    for (std::size_t idx = 0; idx < items; idx++)
                    {
                        queue.push_back(currentObject->at(items - 1 - idx));
                    }
                }
                else if (type == CborBase::TypeMap)
                {
                    // write tag if set
                    uint32_t subtag = currentObject->getTag();
                    if (subtag != TypeUnassigned)
                    {
                        written += writeTypeAndValue(&destination[written], maxLength - written, TypeTag, subtag);
                    }

                    uint32_t items = currentObject->getSize();
                    written += CborBase::writeTypeAndValue(&destination[written], maxLength - written, CborBase::TypeMap, items);

                    // dump map to FILO queue
                    for (std::size_t idx = 0; idx < items; idx++)
                    {
                        queue.push_back(currentObject->value(items - 1 - idx));
                        queue.push_back(currentObject->key(items - 1 - idx));
                    }
                }
                else
                {
                    written += currentObject->writeCBOR(&destination[written], maxLength - written);
                }
            }
        }
    }

    return written;
}

void CborBase::printQueue(std::list<CborBase*> queue)
{
    std::list<uint32_t> progress;
    uint32_t units = queue.size();
    uint32_t indentation = 1;

    while (queue.size() > 0)
    {
        CborBase* currentObject = queue.back();
        queue.pop_back();

        if (currentObject)
        {
            units--;

            // indent
            for (std::size_t idx = 0; idx < indentation; idx++)
            {
                printf("\t");
            }

            //
            uint8_t type = currentObject->getType();

            if (type == CborBase::TypeArray)
            {

                // print tag if set
                uint32_t subtag = currentObject->getTag();
                if (subtag != TypeUnassigned)
                {
                    printf("[%lu] ", subtag);
                }

                uint32_t items = currentObject->getSize();
                printf("Array: %lu\r\n", items);

                if (items > 0)
                {
                    // dump array to FILO queue
                    for (std::size_t idx = 0; idx < items; idx++)
                    {
                        queue.push_back(currentObject->at(items - 1 - idx));
                    }

                    // fix indentation
                    progress.push_back(units);
                    units = items;
                    indentation++;
                }
            }
            else if (type == CborBase::TypeMap)
            {
                // write tag if set
                uint32_t subtag = currentObject->getTag();
                if (subtag != TypeUnassigned)
                {
                    printf("[%lu] ", subtag);
                }

                uint32_t items = currentObject->getSize();
                printf("Map: %lu\r\n", items);

                if (items > 0)
                {
                    // dump map to FILO queue
                    for (std::size_t idx = 0; idx < items; idx++)
                    {
                        queue.push_back(currentObject->value(items - 1 - idx));
                        queue.push_back(currentObject->key(items - 1 - idx));
                    }

                    // fix indentation
                    progress.push_back(units);
                    units = 2 * items;
                    indentation++;
                }
            }
            else
            {
                currentObject->print();
            }

            while ((units == 0) && (progress.size() > 0))
            {
                units = progress.back();
                progress.pop_back();
                indentation--;
            }
        }
    }
}

uint8_t CborBase::writeTypeAndValue(uint8_t* destination, uint32_t maxLength, uint8_t majorType, uint32_t value)
{
    if (destination)
    {
        if (majorType < TypeSpecial)
        {
            uint8_t majorTypeHigh = majorType << 5;

            // value fits in one byte
            if (value <= 23)
            {
                if (maxLength >= 1)
                {
                    destination[0] = majorTypeHigh | value;

                    return 1;
                }
            }
            // value fits in two bytes
            else if (value <= 0xFF)
            {
                if (maxLength >= 2)
                {
                    destination[0] = majorTypeHigh | 24;
                    destination[1] = value;

                    return 2;
                }
            }
            // value fits in three bytes
            else if (value <= 0xFFFF)
            {
                if (maxLength >= 3)
                {
                    destination[0] = majorTypeHigh | 25;
                    destination[1] = value >> 8;
                    destination[2] = value;

                    return 3;
                }
            }
            // value fits in four bytes
            else
            {
                if (maxLength >= 5)
                {
                    destination[0] = majorTypeHigh | 26;
                    destination[1] = value >> 24;
                    destination[2] = value >> 16;
                    destination[3] = value >> 8;
                    destination[4] = value;

                    return 5;
                }
            }
        }
    }

    return 0;
}

uint32_t CborBase::writeBytes(uint8_t* destination, uint32_t maxLength, const uint8_t* source, uint32_t length)
{
    if ((destination) && (source) && (length <= maxLength))
    {
        for (std::size_t idx = 0; idx < length; idx++)
        {
            destination[idx] = source[idx];
        }

        return length;
    }

    return 0;
}

void CborBase::print()
{
    // write tag if set
    if (tag != TypeUnassigned)
    {
        printf("(%lu) ", tag);
    }

    printf("null\r\n");
}
