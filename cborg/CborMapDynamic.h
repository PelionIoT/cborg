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

#ifndef __CBOR_MAP_DYNAMIC_H__
#define __CBOR_MAP_DYNAMIC_H__

#include <stdint.h>
#include <cstddef>
#include <list>
#include <vector>

#include "cborg/CborBase.h"
#include "cborg/CborString.h"
#include "cborg/CborInteger.h"

#include <stdio.h>

class CborMapDynamic : public CborBase
{
public:
    CborMapDynamic()
        :   CborBase(TypeMap, TypeUndefined),
            garbagebin(),
            internalKey(),
            internalValue()
    {}

    ~CborMapDynamic()
    {
        while (garbagebin.size() > 0)
        {
            CborBase* object = garbagebin.back();

            object->print();

            garbagebin.pop_back();
            delete object;
        }
    }

    bool insert(CborBase& key, CborBase& value)
    {
        internalKey.push_back(&key);
        internalValue.push_back(&value);

        return true;
    }

    bool insert(CborBase* key, CborBase* value)
    {
        internalKey.push_back(key);
        internalValue.push_back(value);

        return true;
    }

    /*
    */
    template <std::size_t I>
    bool insert(const char (&key)[I], CborBase& value)
    {
        CborBase* object = new CborString(key);

        garbagebin.push_back(object);
        internalKey.push_back(object);
        internalValue.push_back(&value);

        return true;
    }

    template <std::size_t I>
    bool insert(const char (&key)[I], CborBase* value)
    {
        CborBase* object = new CborString(key);

        garbagebin.push_back(object);
        internalKey.push_back(object);
        internalValue.push_back(value);

        return true;
    }

    /*
    */
    bool insert(uint32_t key, CborBase& value)
    {
        CborBase* object = new CborInteger(key);

        garbagebin.push_back(object);
        internalKey.push_back(object);
        internalValue.push_back(&value);

        return true;
    }

    bool insert(uint32_t key, CborBase* value)
    {
        CborBase* object = new CborInteger(key);

        garbagebin.push_back(object);
        internalKey.push_back(object);
        internalValue.push_back(value);

        return true;
    }


    virtual CborBase* key(std::size_t index)
    {
        if (index < internalKey.size())
        {
            return internalKey.at(index);
        }

        return &CborNull;
    }

    virtual CborBase* value(std::size_t index)
    {
        if (index < internalValue.size())
        {
            return internalValue.at(index);
        }

        return &CborNull;
    }

    /*************************************************************************/
    /* Container                                                             */
    /*************************************************************************/

    virtual uint32_t getLength() const
    {
        return internalValue.size();
    }

    virtual uint32_t getSize() const
    {
        return internalValue.size();
    }

    /*************************************************************************/
    /* Encode                                                                */
    /*************************************************************************/

    virtual uint32_t writeCBOR(uint8_t* destination, uint32_t maxLength)
    {
        std::size_t size = internalValue.size();
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
        std::size_t size = internalValue.size();

        if (tag != TypeUnassigned)
        {
            printf("[%lu] ", tag);
        }

        printf("Map: %u\r\n", size);

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
    std::list<CborBase*> garbagebin;
    std::vector<CborBase*> internalKey;
    std::vector<CborBase*> internalValue;
};

#endif // __CBOR_MAP_DYNAMIC_H__
