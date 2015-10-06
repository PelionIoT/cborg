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

#ifndef __CBOR_ARRAY_DYNAMIC_H__
#define __CBOR_ARRAY_DYNAMIC_H__

#include <stdint.h>
#include <cstddef>
#include <list>
#include <vector>

#include "cborg/CborBase.h"
#include "cborg/CborString.h"
#include "cborg/CborInteger.h"

#include <stdio.h>


class CborArrayDynamic : public CborBase
{
public:
    CborArrayDynamic()
        :   CborBase(TypeArray, TypeUndefined),
            garbagebin(),
            internalArray()
    {}

    ~CborArrayDynamic()
    {
        while (garbagebin.size() > 0)
        {
            CborBase* object = garbagebin.back();

            object->print();

            garbagebin.pop_back();
            delete object;
        }
    }

    bool insert(CborBase& item)
    {
        internalArray.push_back(&item);

        return true;
    }

    bool insert(CborBase* item)
    {
        internalArray.push_back(item);

        return true;
    }

    /*
    */
    template <std::size_t I>
    bool insert(const char (&item)[I])
    {
        CborBase* object = new CborString(item);

        garbagebin.push_back(object);
        internalArray.push_back(object);

        return true;
    }

    bool insert(uint32_t item)
    {
        CborBase* object = new CborInteger(item);

        garbagebin.push_back(object);
        internalArray.push_back(object);

        return true;
    }

    bool insert(int32_t item)
    {
        CborBase* object = new CborInteger(item);

        garbagebin.push_back(object);
        internalArray.push_back(object);

        return true;
    }

    /*
    */

    virtual CborBase* at(std::size_t index)
    {
        if (index < internalArray.size())
        {
            return internalArray.at(index);
        }

        return &CborNull;
    }

    /*************************************************************************/
    /* Container                                                             */
    /*************************************************************************/

    virtual uint32_t getLength() const
    {
        return internalArray.size();
    }

    virtual uint32_t getSize() const
    {
        return internalArray.size();
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
            std::size_t size = internalArray.size();

            // write tag if set
            if (tag != TypeUnassigned)
            {
                written += writeTypeAndValue(destination, maxLength, TypeTag, tag);
            }

            written += CborBase::writeTypeAndValue(&destination[written], maxLength - written, TypeArray, size);

            // dump array to FILO queue
            for (std::size_t idx = 0; idx < size; idx++)
            {
                // use at-function to ensure integrity
                queue.push_back(internalArray.at(size - 1 - idx));
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
        std::size_t size = internalArray.size();

        if (tag != TypeUnassigned)
        {
            printf("[%lu] ", tag);
        }

        printf("Array: %u\r\n", size);

        if (size > 0)
        {
            // dump array to FILO queue
            std::list<CborBase*> queue;

            for (std::size_t idx = 0; idx < size; idx++)
            {
                queue.push_back(internalArray.at(size - 1 - idx));
            }

            CborBase::printQueue(queue);
        }
    }

private:
    std::list<CborBase*> garbagebin;
    std::vector<CborBase*> internalArray;
};

#endif // __CBOR_ARRAY_DYNAMIC_H__
