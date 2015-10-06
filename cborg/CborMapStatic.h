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

#ifndef __CBOR_MAP_STATIC_H__
#define __CBOR_MAP_STATIC_H__

#include <stdint.h>
#include <cstddef>
#include <list>

#include "cborg/CborBase.h"
#include "cborg/CborString.h"

#include <stdio.h>

template <std::size_t I>
class CborMapStatic : public CborBase
{
public:
    CborMapStatic()
        :   CborBase(TypeMap, TypeUndefined),
            size(0)
    {}

    bool insert(CborBase& key, CborBase& value)
    {
        if (size < I)
        {
            internalKey[size] = &key;
            internalValue[size] = &value;
            size++;

            return true;
        }

        return false;
    }

    bool insert(CborBase* key, CborBase* value)
    {
        if (size < I)
        {
            internalKey[size] = key;
            internalValue[size] = value;
            size++;

            return true;
        }

        return false;
    }

    virtual CborBase* key(std::size_t index)
    {
        if (index < size)
        {
            return internalKey[index];
        }

        return &CborNull;
    }

    virtual CborBase* value(std::size_t index)
    {
        if (index < size)
        {
            return internalValue[index];
        }

        return &CborNull;
    }

    /*************************************************************************/
    /* Container                                                             */
    /*************************************************************************/

    virtual uint32_t getLength() const
    {
        return size;
    }

    virtual uint32_t getSize() const
    {
        return size;
    }

    /*************************************************************************/
    /* Encode                                                                */
    /*************************************************************************/

    virtual uint32_t writeCBOR(uint8_t* destination, uint32_t maxLength)
    {
        uint32_t written = 0;

        if (destination)
        {
            std::list<CborBase*> queue;

            // write tag if set
            if (tag != TypeUnassigned)
            {
                written += writeTypeAndValue(destination, maxLength, TypeTag, tag);
            }

            written += CborBase::writeTypeAndValue(&destination[written], maxLength, TypeMap, size);

            // dump array to FILO queue
            for (std::size_t idx = 0; idx < size; idx++)
            {
                queue.push_back(value(size - 1 - idx));
                queue.push_back(key(size - 1 - idx));
            }

            written += CborBase::writeQueue(&destination[written], maxLength - written, queue);
        }

        return written;
    }

    /*************************************************************************/
    /* Debug                                                                 */
    /*************************************************************************/

    virtual void print()
    {
        if (tag != TypeUnassigned)
        {
            printf("[%lu] ", tag);
        }

        printf("Map: %lu\r\n", size);

        if (size > 0)
        {
            // dump array to FILO queue
            std::list<CborBase*> queue;

            for (std::size_t idx = 0; idx < size; idx++)
            {
                queue.push_back(value(size - 1 - idx));
                queue.push_back(key(size - 1 - idx));
            }

            CborBase::printQueue(queue);
        }
    }

private:
    CborBase* internalKey[I];
    CborBase* internalValue[I];
    uint32_t size;
};

#endif // __CBOR_MAP_STATIC_H__
