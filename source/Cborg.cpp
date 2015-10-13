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

#include "cborg/Cborg.h"

#include <list>
#include <stdio.h>

#if 0
#include <stdio.h>
#define DEBUG_PRINTF(...) { printf(__VA_ARGS__); }
#else
#define DEBUG_PRINTF(...)
#endif


Cborg::Cborg()
    :   cbor(NULL),
        maxLength(0)
{}

Cborg::Cborg(const uint8_t* _cbor, std::size_t _length)
    :   cbor(_cbor),
        maxLength(_length)
{}


bool Cborg::getCBOR(const uint8_t** pointer, uint32_t* length)
{
    // decode current header
    CborgHeader head;
    head.decode(cbor);

    uint8_t type = head.getMajorType();
    uint8_t simple = head.getMinorType();

    *pointer = cbor;

    if (type == CborBase::TypeUnassigned)
    {
        return false;
    }
    else if ((type == CborBase::TypeMap) || (type == CborBase::TypeArray)
        || (((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
                    && (simple == CborBase::TypeIndefinite)))
    {
        // list keeps track of the current level
        std::list<uint32_t> list;

        // skip first header
        std::size_t progress = head.getLength();
        uint32_t units = head.getValue();

        // maps contain key-value pairs, double the number of units in container
        if (type == CborBase::TypeMap)
        {
            units *= 2;
        }
        else if (simple == CborBase::TypeIndefinite)
        {
            units = 0xFFFFFFFF;
        }

        // iterate through cbor encoded buffer
        // stop when maximum length is reached or
        // the current container is finished
        while (progress < maxLength)
        {
            // decrement unit count unless set to indefinite
            if (units != 0xFFFFFFFF)
            {
                units--;
            }

            // decode header for cbor object currently pointed to
            head.decode(&cbor[progress]);

            type = head.getMajorType();
            simple = head.getMinorType();

            // if object is a container type (map or array), push remaining units onto the stack (list)
            // and set units to the number of elements in the new container.
            if (type == CborBase::TypeMap)
            {
                if (simple == CborBase::TypeIndefinite)
                {
                    list.push_back(units);
                    units = 0xFFFFFFFF;
                }
                else if (head.getValue() > 0)
                {
                    list.push_back(units);
                    units = 2 * head.getValue();
                }
            }
            else if (type == CborBase::TypeArray)
            {
                if (simple == CborBase::TypeIndefinite)
                {
                    list.push_back(units);
                    units = 0xFFFFFFFF;
                }
                else if (head.getValue() > 0)
                {
                    list.push_back(units);
                    units = head.getValue();
                }
            }
            else if (((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
                    && (simple == CborBase::TypeIndefinite))
            {
                list.push_back(units);
                units = 0xFFFFFFFF;
            }

            // increment progress based on cbor object size
            if (((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
                    && (simple != CborBase::TypeIndefinite))
            {
                progress += head.getLength() + head.getValue();
            }
            else
            {
                progress += head.getLength();
            }

            // finished processing all elements in the current container object
            // step back up one level by popping remaining elements from the stack (list)
            // do it in a while loop since we might have to step back up multiple times
            while ((units == 0) || ((type == CborBase::TypeSpecial) && (simple == CborBase::TypeIndefinite)))
            {
                type = CborBase::TypeSpecial;
                simple = CborBase::TypeNull;

                if (list.size() > 0)
                {
                    // pop from stack
                    units = list.back();
                    list.pop_back();
                }
                else
                {
                    // stack is empty, means we have reached the end of the current container
                    *length = progress;

                    return true;
                }
            }
        }

        return false;
    }
    else if ((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
    {
        // return header and length
        *length = head.getLength() + head.getValue();
    }
    else
    {
        // return header
        *length = head.getLength();
    }

    return true;
}

uint32_t Cborg::getCBORLength()
{
    // decode current header
    CborgHeader head;
    head.decode(cbor);

    uint8_t type = head.getMajorType();
    uint8_t simple = head.getMinorType();

    if (type == CborBase::TypeUnassigned)
    {
        return 0;
    }
    else if ((type == CborBase::TypeMap) || (type == CborBase::TypeArray)
        || (((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
                    && (simple == CborBase::TypeIndefinite)))
    {
        // list keeps track of the current level
        std::list<uint32_t> list;

        // skip first header
        std::size_t progress = head.getLength();
        uint32_t units = head.getValue();

        // maps contain key-value pairs, double the number of units in container
        if (type == CborBase::TypeMap)
        {
            units *= 2;
        }
        else if (simple == CborBase::TypeIndefinite)
        {
            units = 0xFFFFFFFF;
        }

        // iterate through cbor encoded buffer
        // stop when maximum length is reached or
        // the current container is finished
        while (progress < maxLength)
        {
            // decrement unit count unless set to indefinite
            if (units != 0xFFFFFFFF)
            {
                units--;
            }

            // decode header for cbor object currently pointed to
            head.decode(&cbor[progress]);

            type = head.getMajorType();
            simple = head.getMinorType();

            // if object is a container type (map or array), push remaining units onto the stack (list)
            // and set units to the number of elements in the new container.
            if (type == CborBase::TypeMap)
            {
                if (simple == CborBase::TypeIndefinite)
                {
                    list.push_back(units);
                    units = 0xFFFFFFFF;
                }
                else if (head.getValue() > 0)
                {
                    list.push_back(units);
                    units = 2 * head.getValue();
                }
            }
            else if (type == CborBase::TypeArray)
            {
                if (simple == CborBase::TypeIndefinite)
                {
                    list.push_back(units);
                    units = 0xFFFFFFFF;
                }
                else if (head.getValue() > 0)
                {
                    list.push_back(units);
                    units = head.getValue();
                }
            }
            else if (((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
                    && (simple == CborBase::TypeIndefinite))
            {
                list.push_back(units);
                units = 0xFFFFFFFF;
            }

            // increment progress based on cbor object size
            if (((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
                    && (simple != CborBase::TypeIndefinite))
            {
                progress += head.getLength() + head.getValue();
            }
            else
            {
                progress += head.getLength();
            }

            // finished processing all elements in the current container object
            // step back up one level by popping remaining elements from the stack (list)
            // do it in a while loop since we might have to step back up multiple times
            while ((units == 0) || ((type == CborBase::TypeSpecial) && (simple == CborBase::TypeIndefinite)))
            {
                type = CborBase::TypeSpecial;
                simple = CborBase::TypeNull;

                if (list.size() > 0)
                {
                    // pop from stack
                    units = list.back();
                    list.pop_back();
                }
                else
                {
                    // stack is empty, means we have reached the end of the current container
                    return progress;
                }
            }
        }

        return progress;
    }
    else if ((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
    {
        // return header and length
        return head.getLength() + head.getValue();
    }
    else
    {
        // return header
        return head.getLength();
    }
}

Cborg Cborg::find(int32_t key) const
{
    CborgHeader head;
    head.decode(cbor);

    uint8_t type = head.getMajorType();
    uint8_t simple = head.getMinorType();

    uint32_t units = 2 * head.getValue();
    units = (simple == CborBase::TypeIndefinite) ? 0xFFFFFFFF : units;

    // only continue if type is Cbor Map and the map is not empty
    if ((type != CborBase::TypeMap) || (units == 0))
    {
        return Cborg(NULL, 0);
    }

    // got map, look for key
    std::list<uint32_t> list;
    bool gotKey = false;

    // skip map header
    std::size_t progress = head.getLength();

    // iterate through cbor encoded buffer
    // stop when maximum length is reached or
    // the current map is finished
    while (progress < maxLength)
    {
        // decrement unit count unless set to indefinite
        if (units != 0xFFFFFFFF)
        {
            units--;
        }

        // decode header for cbor object currently pointed to
        head.decode(&cbor[progress]);

        type = head.getMajorType();
        simple = head.getMinorType();

        // if object is a container type (map or array), push remaining units onto the stack (list)
        // and set units to the number of elements in the new container.
        if (type == CborBase::TypeMap)
        {
            gotKey = false;

            if (simple == CborBase::TypeIndefinite)
            {
                list.push_back(units);
                units = 0xFFFFFFFF;
            }
            else if (head.getValue() > 0)
            {
                list.push_back(units);
                units = 2 * head.getValue();
            }
        }
        else if (type == CborBase::TypeArray)
        {
            gotKey = false;

            if (simple == CborBase::TypeIndefinite)
            {
                list.push_back(units);
                units = 0xFFFFFFFF;
            }
            else if (head.getValue() > 0)
            {
                list.push_back(units);
                units = head.getValue();
            }
        }
        else if (((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
                && (simple == CborBase::TypeIndefinite))
        {
            gotKey = false;

            list.push_back(units);
            units = 0xFFFFFFFF;
        }
        else
        {
            // object is not a container type
            // if object is on the same level as the top level map then process content
            // otherwise igonre and move on
            if (list.size() == 0)
            {
                // if previous object was a key, this is a value. Skip it.
                if (gotKey)
                {
                    gotKey = false;
                }
                else
                {
                    // assume keys are cbor integers.
                    bool found = false;

                    if (type == CborBase::TypeUnsigned)
                    {
                        if (head.getValue() == key)
                        {
                            found = true;
                        }
                    }
                    else if (type == CborBase::TypeNegative)
                    {
                        if ((-1 - head.getValue()) == key)
                        {
                            found = true;
                        }
                    }

                    if (found)
                    {
                        // update progress to point to next object
                        progress += head.getLength();

                        // return new Cborg object based on object pointer and max length
                        return Cborg(&cbor[progress], maxLength - progress);
                    }

                    // this object is a key, but the key didn't match
                    // signal that the next object is a value
                    gotKey = true;
                }
            }
        }

        // increment progress based on cbor object size
        if (((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
                && (simple != CborBase::TypeIndefinite))
        {
            progress += head.getLength() + head.getValue();
        }
        else
        {
            progress += head.getLength();
        }

        // finished processing all elements in the current container object
        // step back up one level by popping remaining elements from the stack (list)
        // do it in a while loop since we might have to step back up multiple times
        while ((units == 0) || ((type == CborBase::TypeSpecial) && (simple == CborBase::TypeIndefinite)))
        {
            type = CborBase::TypeSpecial;
            simple = CborBase::TypeNull;

            if (list.size() > 0)
            {
                // pop from stack
                units = list.back();
                list.pop_back();
            }
            else
            {
                // stack is empty, means we have reached the end of the current container
                // return a Cbor null object
                return Cborg(NULL, 0);
            }
        }
    }

    // key not found, retun null object
    return Cborg(NULL, 0);
}

Cborg Cborg::find(const char* key, std::size_t keyLength) const
{
    CborgHeader head;
    head.decode(cbor);

    uint8_t type = head.getMajorType();
    uint8_t simple = head.getMinorType();

    uint32_t units = 2 * head.getValue();
    units = (simple == CborBase::TypeIndefinite) ? 0xFFFFFFFF : units;

    // only continue if type is Cbor Map, key is not NULL, and the map is not empty
    if ((type != CborBase::TypeMap) || (key == NULL) || (units == 0))
    {
        return Cborg(NULL, 0);
    }

    // got map, look for key
    std::list<uint32_t> list;
    bool gotKey = false;

    // skip map header
    std::size_t progress = head.getLength();

    // iterate through cbor encoded buffer
    // stop when maximum length is reached or
    // the current map is finished
    while (progress < maxLength)
    {
        // decrement unit count unless set to indefinite
        if (units != 0xFFFFFFFF)
        {
            units--;
        }

        // decode header for cbor object currently pointed to
        head.decode(&cbor[progress]);

        type = head.getMajorType();
        simple = head.getMinorType();

        // if object is a container type (map or array), push remaining units onto the stack (list)
        // and set units to the number of elements in the new container.
        if (type == CborBase::TypeMap)
        {
            gotKey = false;

            if (simple == CborBase::TypeIndefinite)
            {
                list.push_back(units);
                units = 0xFFFFFFFF;
            }
            else if (head.getValue() > 0)
            {
                list.push_back(units);
                units = 2 * head.getValue();
            }
        }
        else if (type == CborBase::TypeArray)
        {
            gotKey = false;

            if (simple == CborBase::TypeIndefinite)
            {
                list.push_back(units);
                units = 0xFFFFFFFF;
            }
            else if (head.getValue() > 0)
            {
                list.push_back(units);
                units = head.getValue();
            }
        }
        else if (((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
                && (simple == CborBase::TypeIndefinite))
        {
            gotKey = false;

            list.push_back(units);
            units = 0xFFFFFFFF;
        }
        else
        {
            // object is not a container type
            // if object is on the same level as the top level map then process content
            // otherwise igonre and move on
            if (list.size() == 0)
            {
                // if previous object was a key, this is a value. Skip it.
                if (gotKey)
                {
                    gotKey = false;
                }
                else
                {
                    // if object is a string and the length matches the key length
                    // do a byte-by-byte comparison
                    if (type == CborBase::TypeString)
                    {
                        if (head.getValue() == keyLength)
                        {
                            bool found = true;

                            for (std::size_t idx = 0; idx < keyLength; idx++)
                            {
                                // break out of loop if string is different from the key
                                if (key[idx] != cbor[progress + head.getLength() + idx])
                                {
                                    found = false;
                                    break;
                                }
                            }

                            if (found)
                            {
                                // update progress to point to next object
                                progress += head.getLength() + head.getValue();

                                // return new Cborg object based on object pointer and max length
                                return Cborg(&cbor[progress], maxLength - progress);
                            }
                        }
                    }

                    // this object is a key, but the key didn't match
                    // signal that the next object is a value
                    gotKey = true;
                }
            }
        }

        // increment progress based on cbor object size
        if (((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
                && (simple != CborBase::TypeIndefinite))
        {
            progress += head.getLength() + head.getValue();
        }
        else
        {
            progress += head.getLength();
        }

        // finished processing all elements in the current container object
        // step back up one level by popping remaining elements from the stack (list)
        // do it in a while loop since we might have to step back up multiple times
        while ((units == 0) || ((type == CborBase::TypeSpecial) && (simple == CborBase::TypeIndefinite)))
        {
            type = CborBase::TypeSpecial;
            simple = CborBase::TypeNull;

            if (list.size() > 0)
            {
                // pop from stack
                units = list.back();
                list.pop_back();
            }
            else
            {
                // stack is empty, means we have reached the end of the current container
                // return a Cbor null object
                return Cborg(NULL, 0);
            }
        }
    }

    // key not found, retun null object
    return Cborg(NULL, 0);
}

Cborg Cborg::at(std::size_t index) const
{
    CborgHeader head;
    head.decode(cbor);

    uint8_t type = head.getMajorType();
    uint8_t simple = head.getMinorType();

    // set units to elements in array
    int32_t units = head.getValue();
    units = (simple == CborBase::TypeIndefinite) ? 0xFFFFFFFF : units;

    // only continue if container is Cbor Map, not empty, and index is within bounds
    if ((type != CborBase::TypeArray) || (units == 0) || (index >= units))
    {
        return Cborg(NULL, 0);
    }

    // got array, look for index
    std::list<uint32_t> list;
    std::size_t currentIndex = 0;

    // skip array header
    std::size_t progress = head.getLength();

    // iterate through cbor encoded buffer
    // stop when maximum length is reached or
    // the current array is finished
    while (progress < maxLength)
    {
        // compare current array index with the one sought for and return if found
        if (currentIndex == index)
        {
            return Cborg(&cbor[progress], maxLength - progress);
        }
        else
        {
            // decrement unit count unless set to indefinite
            if (units != 0xFFFFFFFF)
            {
                units--;
            }

            // decode header for cbor object currently pointed to
            head.decode(&cbor[progress]);

            type = head.getMajorType();
            simple = head.getMinorType();

            // if object is a container type (map or array), push remaining units onto the stack (list)
            // and set units to the number of elements in the new container.
            if (type == CborBase::TypeMap)
            {
                if (simple == CborBase::TypeIndefinite)
                {
                    list.push_back(units);
                    units = 0xFFFFFFFF;
                }
                else if (head.getValue() > 0)
                {
                    list.push_back(units);
                    units = 2 * head.getValue();
                }
            }
            else if (type == CborBase::TypeArray)
            {
                if (simple == CborBase::TypeIndefinite)
                {
                    list.push_back(units);
                    units = 0xFFFFFFFF;
                }
                else if (head.getValue() > 0)
                {
                    list.push_back(units);
                    units = head.getValue();
                }
            }
            else if (((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
                    && (simple == CborBase::TypeIndefinite))
            {
                list.push_back(units);
                units = 0xFFFFFFFF;
            }

            // increment progress based on cbor object size
            if (((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
                    && (simple != CborBase::TypeIndefinite))
            {
                progress += head.getLength() + head.getValue();
            }
            else
            {
                progress += head.getLength();
            }

            // finished processing all elements in the current container object
            // step back up one level by popping remaining elements from the stack (list)
            // do it in a while loop since we might have to step back up multiple times
            while ((units == 0) || ((type == CborBase::TypeSpecial) && (simple == CborBase::TypeIndefinite)))
            {
                type = CborBase::TypeSpecial;
                simple = CborBase::TypeNull;

                if (list.size() > 0)
                {
                    // pop from stack
                    units = list.back();
                    list.pop_back();
                }
                else
                {
                    // stack is empty, means we have reached the end of the current container
                    // return a Cbor null object
                    return Cborg(NULL, 0);
                }
            }

            // if element is on the top level, increment index counter
            if (list.size() == 0)
            {
                currentIndex++;
            }
        }
    }

    // index not found, return null object
    return Cborg(NULL, 0);
}

uint32_t Cborg::getSize() const
{
    CborgHeader head;
    head.decode(cbor);

    uint8_t type = head.getMajorType();
    uint8_t simple = head.getMinorType();

    if ((type == CborBase::TypeMap)
        || (type == CborBase::TypeArray)
        || (type == CborBase::TypeString)
        || (type == CborBase::TypeBytes))
    {
        if (simple == CborBase::TypeIndefinite)
        {
            return 0xFFFFFFFF;
        }
        else
        {
            return head.getValue();
        }
    }
    else
    {
        return 0;
    }
}


bool Cborg::getUnsigned(uint32_t* integer) const
{
    CborgHeader head;
    head.decode(cbor);

    if (head.getMajorType() == CborBase::TypeUnsigned)
    {
        *integer = head.getValue();

        return true;
    }
    else
    {
        return false;
    }
}

bool Cborg::getNegative(int32_t* integer) const
{
    CborgHeader head;
    head.decode(cbor);

    if (head.getMajorType() == CborBase::TypeNegative)
    {
        *integer = -1 - head.getValue();

        return true;
    }
    else
    {
        return false;
    }
}

bool Cborg::getBytes(const uint8_t** pointer, uint32_t* length) const
{
    CborgHeader head;
    head.decode(cbor);

    if (head.getMajorType() == CborBase::TypeBytes)
    {
        *pointer = &cbor[head.getLength()];
        *length = head.getValue();

        return true;
    }
    else
    {
        return false;
    }
}

bool Cborg::getString(const char** pointer, uint32_t* length) const
{
    CborgHeader head;
    head.decode(cbor);

    if (head.getMajorType() == CborBase::TypeString)
    {
        *pointer = (const char*) &cbor[head.getLength()];
        *length = head.getValue();

        return true;
    }
    else
    {
        return false;
    }
}

bool Cborg::getString(std::string& str) const
{
    CborgHeader head;
    head.decode(cbor);

    if (head.getMajorType() == CborBase::TypeString)
    {
        str.assign((const char*) &cbor[head.getLength()], head.getValue());

        return true;
    }
    else
    {
        return false;
    }
}

/*****************************************************************************/
/* Header related                                                            */
/*****************************************************************************/

uint32_t Cborg::getTag() const
{
    CborgHeader head;
    head.decode(cbor);

    return head.getTag();
}

uint8_t Cborg::getType() const
{
    CborgHeader head;
    head.decode(cbor);

    return head.getMajorType();
}

uint8_t Cborg::getMinorType() const
{
    CborgHeader head;
    head.decode(cbor);

    return head.getMinorType();
}


/*****************************************************************************/
/* Debug related                                                             */
/*****************************************************************************/

void Cborg::print() const
{
    CborgHeader head;
    std::size_t progress = 0;

    std::list<uint32_t> list;
    uint32_t units = 1;

    while (progress < maxLength)
    {
        // decrement unit count unless set to indefinite
        if (units != 0xFFFFFFFF)
        {
            units--;
        }

        head.decode(&cbor[progress]);

        /* semantic tag */
        uint32_t tag = head.getTag();
        uint8_t type = head.getMajorType();
        uint8_t simple = head.getMinorType();

        if ((type != CborBase::TypeSpecial) || (simple != CborBase::TypeIndefinite))
        {
            for (std::size_t indent = 0; indent < list.size(); indent++)
            {
                printf("\t");
            }
        }

        if (tag != CborBase::TypeUnassigned)
        {
            printf("[%u] ", tag);
        }

        /* container object */
        if (type == CborBase::TypeMap)
        {
            if (simple == CborBase::TypeIndefinite)
            {
                printf("Map:\r\n");

                list.push_back(units);
                units = 0xFFFFFFFF;
            }
            else if (head.getValue() > 0)
            {
                printf("Map: %u\r\n", head.getValue());

                list.push_back(units);
                units = 2 * head.getValue();
            }
        }
        else if (type == CborBase::TypeArray)
        {
            if (simple == CborBase::TypeIndefinite)
            {
                printf("Array:\r\n");

                list.push_back(units);
                units = 0xFFFFFFFF;
            }
            else if (head.getValue() > 0)
            {
                printf("Array: %u\r\n", head.getValue());

                list.push_back(units);
                units = head.getValue();
            }
        }
        else if ((type == CborBase::TypeBytes) && (simple == CborBase::TypeIndefinite))
        {
            printf("Bytes:\r\n");

            list.push_back(units);
            units = 0xFFFFFFFF;
        }
        else if ((type == CborBase::TypeString) && (simple == CborBase::TypeIndefinite))
        {
            printf("String:\r\n");

            list.push_back(units);
            units = 0xFFFFFFFF;
        }
        else
        {
            switch(type)
            {
                case CborBase::TypeUnsigned:
                    {
                        Cborg object(&cbor[progress], maxLength - progress);
                        uint32_t integer = 0;
                        bool result = object.getUnsigned(&integer);

                        if (result)
                        {
                            printf("%u\r\n", integer);
                        }
                        else
                        {
                            printf("\r\n");
                        }
                    }
                    break;

                case CborBase::TypeNegative:
                    {
                        Cborg object(&cbor[progress], maxLength - progress);
                        int32_t integer = 0;
                        bool result = object.getNegative(&integer);

                        if (result)
                        {
                            printf("%d\r\n", integer);
                        }
                        else
                        {
                            printf("\r\n");
                        }
                    }
                    break;

                case CborBase::TypeBytes:
                    {
                        Cborg object(&cbor[progress], maxLength - progress);
                        const uint8_t* pointer;
                        uint32_t length;

                        bool result = object.getBytes(&pointer, &length);

                        if (result)
                        {
                            for (std::size_t idx = 0; idx < length; idx++)
                            {
                                printf("%02X", pointer[idx]);
                            }
                        }
                        printf("\r\n");
                    }
                    break;

                case CborBase::TypeString:
                    {
                        Cborg object(&cbor[progress], maxLength - progress);
                        const char* pointer;
                        uint32_t length;

                        bool result = object.getString(&pointer, &length);

                        if (result)
                        {
                            for (std::size_t idx = 0; idx < length; idx++)
                            {
                                printf("%c", pointer[idx]);
                            }
                        }
                        printf("\r\n");
                    }
                    break;

                case CborBase::TypeSpecial:
                    {
                        if (simple == CborBase::TypeTrue)
                        {
                            printf("true\r\n");
                        }
                        else if (simple == CborBase::TypeFalse)
                        {
                            printf("false\r\n");
                        }
                        else if (simple == CborBase::TypeNull)
                        {
                            printf("null\r\n");
                        }
                        else if (simple == CborBase::TypeUndefined)
                        {
                            printf("undefined\r\n");
                        }
                        else if (simple == CborBase::TypeHalfFloat)
                        {
                            printf("half float\r\n");
                        }
                        else if (simple == CborBase::TypeSingleFloat)
                        {
                            printf("single float\r\n");
                        }
                        else if (simple == CborBase::TypeDoubleFloat)
                        {
                            printf("doubel float\r\n");
                        }
                    }
                    break;

                default:
                    printf("error\r\n");
                    return;
                    break;
            }
        }

        // increment progress based on cbor object size
        if (((type == CborBase::TypeBytes) || (type == CborBase::TypeString))
                && (simple != CborBase::TypeIndefinite))
        {
            progress += head.getLength() + head.getValue();
        }
        else
        {
            progress += head.getLength();
        }

        // finished processing all elements in the current container object
        // step back up one level by popping remaining elements from the stack (list)
        // do it in a while loop since we might have to step back up multiple times
        while ((units == 0) || ((type == CborBase::TypeSpecial) && (simple == CborBase::TypeIndefinite)))
        {
            type = CborBase::TypeSpecial;
            simple = CborBase::TypeNull;

            if (list.size() > 0)
            {
                units = list.back();
                list.pop_back();
            }
            else
            {
                return;
            }
        }
    }
}


