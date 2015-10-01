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
    CborHeader head;
    head.decode(cbor);
    uint8_t type = head.getMajorType();

    *pointer = cbor;

    if (type == Cborg::TypeUnassigned)
    {
        return false;
    }
    else if ((type == Cborg::TypeMap) || (type == Cborg::TypeArray))
    {
        // list keeps track of the current level
        std::list<uint32_t> list;

        // skip first header
        std::size_t index = head.getLength();
        uint32_t units = head.getValue();

        // maps contain key-value pairs, double the number of units in container
        if (type == Cborg::TypeMap)
        {
            units *= 2;
        }

        // iterate through cbor encoded buffer
        // stop when maximum length is reached or
        // the current container is finished
        while (index < maxLength)
        {
            // decode header for cbor object currently pointed to
            head.decode(&cbor[index]);

            // if object is a container type (map or array), push remaining units onto the stack (list)
            // and set units to the number of elements in the new container.
            if (head.getMajorType() == Cborg::TypeMap)
            {
                DEBUG_PRINTF("%lu: %d ", list.size(), units);
                list.push_back(units);

                units = 2 * head.getValue();
                DEBUG_PRINTF("map: %d\r\n", units);
            }
            else if (head.getMajorType() == Cborg::TypeArray)
            {
                DEBUG_PRINTF("%lu: %d ", list.size(), units);
                list.push_back(units);

                units = head.getValue();
                DEBUG_PRINTF("array: %d\r\n", units);
            }

            // finished with current element, decrement element counter
            units--;

            // increment index based on cbor object size
            if ((head.getMajorType() == Cborg::TypeBytes) || (head.getMajorType() == Cborg::TypeString))
            {
                index += head.getLength() + head.getValue();
            }
            else
            {
                index += head.getLength();
            }

            // finished processing all elements in the current container object
            // step back up one level by popping remaining elements from the stack (list)
            // do it in a while loop since we might have to step back up multiple times
            while (units == 0)
            {
                DEBUG_PRINTF("%lu: done\r\n", list.size());

                if (list.size() > 0)
                {
                    // pop from stack
                    units = list.back();
                    list.pop_back();
                }
                else
                {
                    // stack is empty, means we have reached the end of the current container
                    *length = index;

                    return true;
                }
            }
        }

        return false;
    }
    else if ((type == Cborg::TypeBytes) || (type == Cborg::TypeString))
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



Cborg Cborg::find(const char* key, std::size_t keyLength)
{
    // only continue if type is Cbor Map
    CborHeader head;
    head.decode(cbor);

    if (head.getMajorType() != Cborg::TypeMap)
    {
        return Cborg(NULL, 0);
    }

    // got map, look for key
    std::list<uint32_t> list;
    bool gotKey = false;
    Cborg cborKey;

    // skip map header, set units to elements in map
    std::size_t index = head.getLength();
    uint32_t units = 2 * head.getValue();

    // iterate through cbor encoded buffer
    // stop when maximum length is reached or
    // the current map is finished
    while (index < maxLength)
    {
        // decode header for cbor object currently pointed to
        head.decode(&cbor[index]);

        // if object is a container type (map or array), push remaining units onto the stack (list)
        // and set units to the number of elements in the new container.
        if (head.getMajorType() == Cborg::TypeMap)
        {
            DEBUG_PRINTF("%lu: %d ", list.size(), units);
            list.push_back(units);

            units = 2 * head.getValue();
            DEBUG_PRINTF("map: %d\r\n", units);
        }
        else if (head.getMajorType() == Cborg::TypeArray)
        {
            DEBUG_PRINTF("%lu: %d ", list.size(), units);
            list.push_back(units);

            units = head.getValue();
            DEBUG_PRINTF("array: %d\r\n", units);
        }
        else
        {
            DEBUG_PRINTF("%lu: %d %d %d %u %d\r\n", list.size(), units, head.getTag(), head.getMajorType(), head.getLength(), head.getValue());

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
                    // assume keys are cbor strings.
                    // if object is a string and the length matches the key length
                    // do a byte-by-byte comparison
                    if (head.getMajorType() == Cborg::TypeString)
                    {
                        if (head.getValue() == keyLength)
                        {
                            for (std::size_t idx = 0; idx < keyLength; idx++)
                            {
                                // break out of loop if string is different from the key
                                if (key[idx] != cbor[index + head.getLength() + idx])
                                {
                                    break;
                                }

                                // didn't break out, mean we found key
                                DEBUG_PRINTF("found key!\r\n");

                                // update index to point to next object
                                index += head.getLength() + head.getValue();

                                // return new Cborg object based on object pointer and max length
                                return Cborg(&cbor[index], maxLength - index);
                            }
                        }

                        // this object is a key, but the key didn't match
                        // signal that the next object is a value
                        gotKey = true;
                    }
                }
            }
        }

        // finished with current element, decrement element counter
        units--;

        // finished processing all elements in the current container object
        // step back up one level by popping remaining elements from the stack (list)
        // do it in a while loop since we might have to step back up multiple times
        while (units == 0)
        {
            DEBUG_PRINTF("%lu: done\r\n", list.size());

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

        // increment index based on cbor object size
        if ((head.getMajorType() == Cborg::TypeBytes) || (head.getMajorType() == Cborg::TypeString))
        {
            index += head.getLength() + head.getValue();
        }
        else
        {
            index += head.getLength();
        }
    }

    // key not found, retun null object
    return Cborg(NULL, 0);
}

Cborg Cborg::at(std::size_t entry)
{
    // only continue if type is Cbor Map
    CborHeader head;
    head.decode(cbor);

    if (head.getMajorType() != Cborg::TypeArray)
    {
        return Cborg(NULL, 0);
    }

    // got array, look for entry
    std::list<uint32_t> list;
    std::size_t currentEntry = 0;

    // skip array header, set units to elements in array
    std::size_t index = head.getLength();
    int32_t units = head.getValue();

    // if entry is out of bounce, return null object
    if (entry >= units)
    {
        return Cborg(NULL, 0);
    }

    // iterate through cbor encoded buffer
    // stop when maximum length is reached or
    // the current array is finished
    while (index < maxLength)
    {
        // compare current array index with the one sought for and return if found
        if (currentEntry == entry)
        {
            return Cborg(&cbor[index], maxLength - index);
        }
        else
        {
            // decode header for cbor object currently pointed to
            head.decode(&cbor[index]);

            // if object is a container type (map or array), push remaining units onto the stack (list)
            // and set units to the number of elements in the new container.
            if (head.getMajorType() == Cborg::TypeMap)
            {
                DEBUG_PRINTF("%lu: %d ", list.size(), units);
                list.push_back(units);

                units = 2 * head.getValue();
                DEBUG_PRINTF("map: %d\r\n", units);
            }
            else if (head.getMajorType() == Cborg::TypeArray)
            {
                DEBUG_PRINTF("%lu: %d ", list.size(), units);
                list.push_back(units);

                units = head.getValue();
                DEBUG_PRINTF("array: %d\r\n", units);
            }
            else
            {
                DEBUG_PRINTF("%lu: %d %d %d %u %d\r\n", list.size(), units, head.getTag(), head.getMajorType(), head.getLength(), head.getValue());
            }

            // finished with current element, decrement element counter
            units--;

            // if element is on the top level, increment entry counter
            if (list.size() == 0)
            {
                currentEntry++;
            }

            // finished processing all elements in the current container object
            // step back up one level by popping remaining elements from the stack (list)
            // do it in a while loop since we might have to step back up multiple times
            while (units == 0)
            {
                DEBUG_PRINTF("%lu: done\r\n", list.size());

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

            // increment index based on cbor object size
            if (head.getMajorType() == Cborg::TypeString)
            {
                index += head.getLength() + head.getValue();
            }
            else
            {
                index += head.getLength();
            }
        }
    }

    // entry not found, return null object
    return Cborg(NULL, 0);
}

uint32_t Cborg::getSize() const
{
    CborHeader head;
    head.decode(cbor);

    if ((head.getMajorType() == Cborg::TypeMap) || (head.getMajorType() == Cborg::TypeArray))
    {
        return head.getValue();
    }
    else
    {
        return 0;
    }
}


bool Cborg::getUnsigned(uint32_t* integer) const
{
    CborHeader head;
    head.decode(cbor);

    if (head.getMajorType() == Cborg::TypeUnsigned)
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
    CborHeader head;
    head.decode(cbor);

    if (head.getMajorType() == Cborg::TypeNegative)
    {
        *integer = -1 * head.getValue();

        return true;
    }
    else
    {
        return false;
    }
}

bool Cborg::getBytes(const uint8_t** pointer, uint32_t* length) const
{
    CborHeader head;
    head.decode(cbor);

    if (head.getMajorType() == Cborg::TypeBytes)
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
    CborHeader head;
    head.decode(cbor);

    if (head.getMajorType() == Cborg::TypeString)
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


/*****************************************************************************/
/* Header related                                                            */
/*****************************************************************************/

uint32_t Cborg::getTag() const
{
    CborHeader head;
    head.decode(cbor);

    return head.getTag();
}

uint8_t Cborg::getType() const
{
    CborHeader head;
    head.decode(cbor);

    return head.getMajorType();
}

uint8_t Cborg::getMinorType() const
{
    CborHeader head;
    head.decode(cbor);

    return head.getMinorType();
}


/*****************************************************************************/
/* Debug related                                                             */
/*****************************************************************************/

void Cborg::print()
{
    CborHeader head;
    std::size_t index = 0;
    std::list<uint32_t> list;
    int32_t units = 0;

    while (index < maxLength)
    {
        for (std::size_t indent = 0; indent < list.size(); indent++)
        {
            printf("\t");
        }

        Cborg object(&cbor[index], maxLength - index);

        /* semantic tag */
        uint32_t tag = object.getTag();

        if (tag != Cborg::TypeUnassigned)
        {
            printf("(%u) ", tag);
        }

        /* container object */
        if (object.getType() == Cborg::TypeMap)
        {
            DEBUG_PRINTF("%lu: %d ", list.size(), units);
            printf("map: %d\r\n", object.getSize());

            list.push_back(units);
            units = 2 * object.getSize();
        }
        else if (object.getType() == Cborg::TypeArray)
        {
            DEBUG_PRINTF("%lu: %d ", list.size(), units);
            printf("array: %d\r\n", object.getSize());

            list.push_back(units);
            units = object.getSize();
        }
        else
        {
            switch(object.getType())
            {
                case Cborg::TypeUnsigned:
                    {
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

                case Cborg::TypeNegative:
                    {
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

                case Cborg::TypeBytes:
                    {
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

                case Cborg::TypeString:
                    {
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

                case Cborg::TypeSpecial:
                    {
                        uint8_t simple = object.getMinorType();

                        if (simple == Cborg::TypeTrue)
                        {
                            printf("true\r\n");
                        }
                        else if (simple == Cborg::TypeFalse)
                        {
                            printf("false\r\n");
                        }
                        else if (simple == Cborg::TypeNull)
                        {
                            printf("null\r\n");
                        }
                        else if (simple == Cborg::TypeUndefined)
                        {
                            printf("undefined\r\n");
                        }
                        else if (simple == Cborg::TypeHalfFloat)
                        {
                            printf("half float\r\n");
                        }
                        else if (simple == Cborg::TypeSingleFloat)
                        {
                            printf("single float\r\n");
                        }
                        else if (simple == Cborg::TypeDoubleFloat)
                        {
                            printf("doubel float\r\n");
                        }
                    }
                    break;

                default:
                            break;
            }

            while (units == 0)
            {
                DEBUG_PRINTF("%lu: done\r\n", list.size());

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


        //
        head.decode(&cbor[index]);
        if ((head.getMajorType() == Cborg::TypeString) || (head.getMajorType() == Cborg::TypeBytes))
        {
            index += head.getLength() + head.getValue();
        }
        else
        {
            index += head.getLength();
        }

        units--;
    }
}

