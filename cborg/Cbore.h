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
#include "cborg/CborBase.h"

class Cbore
{
public:
    Cbore();

    Cbore(uint8_t* cbor, std::size_t maxLength);

    std::size_t getLength() const;

    /* Encode methods */
    Cbore& tag(uint32_t tag);



    /*************************************************************************/
    /* Array creation                                                        */
    /*************************************************************************/

    // create array in array
    Cbore& array(std::size_t items);

    // create array in map <integer, array>
    Cbore& array(int32_t key, std::size_t size);

    // create array in map <string, array>
    template <std::size_t I>
    Cbore& array(const char (&key)[I], std::size_t size)
    {
        if ((itemSize(I) + I + itemSize(size)) <= (maxLength - currentLength))
        {
            writeTypeAndValue(CborBase::TypeString, I - 1);
            writeBytes((const uint8_t*) key, I - 1);
            writeTypeAndValue(CborBase::TypeArray, size);
        }

        return *this;
    }

    /*************************************************************************/
    /* Array insertion                                                       */
    /*************************************************************************/

    // write <integer>
    Cbore& item(int32_t value);

    // write <simple type>
    Cbore& item(CborBase::SimpleType_t simpleType);

    // write <string>
    template <std::size_t I>
    Cbore& item(const char (&string)[I])
    {
        if ((itemSize(I) + I) <= (maxLength - currentLength))
        {
            writeTypeAndValue(CborBase::TypeString, I - 1);
            writeBytes((const uint8_t*) string, I - 1);
        }

        return *this;
    }

    // write string, length
    Cbore& item(char* string, std::size_t length);

    /*************************************************************************/
    /* Map creation                                                          */
    /*************************************************************************/

    // create map in array
    Cbore& map(std::size_t items);

    // create map in map
    Cbore& map(int32_t key, std::size_t size);

    // create map in map <string, map>
    template <std::size_t I>
    Cbore& map(const char (&key)[I], std::size_t size)
    {
        if ((itemSize(I) + I + itemSize(size)) <= (maxLength - currentLength))
        {
            writeTypeAndValue(CborBase::TypeString, I - 1);
            writeBytes((const uint8_t*) key, I - 1);
            writeTypeAndValue(CborBase::TypeMap, size);
        }

        return *this;
    }

    /*************************************************************************/
    /* Map insertion                                                         */
    /*************************************************************************/

    // insert <integer, integer>
    Cbore& item(int32_t key, int32_t value);

    // insert <integer, simple type> (Cbor::TypeNull, TypeFalse, TypeTrue)
    Cbore& item(int32_t key, CborBase::SimpleType_t value);

    // insert <integer, string>
    template <std::size_t I>
    Cbore& item(int32_t key, const char (&value)[I])
    {
        if ((itemSize(key) + itemSize(I) + I) <= (maxLength - currentLength))
        {
            if (key < 0)
            {
                writeTypeAndValue(CborBase::TypeNegative, -1 - key);
            }
            else
            {
                writeTypeAndValue(CborBase::TypeUnsigned, key);
            }

            writeTypeAndValue(CborBase::TypeString, I - 1);
            writeBytes((const uint8_t*) value, I - 1);
        }

        return *this;
    }

    // insert <integer, string, length>
    Cbore& item(int32_t key, char* value, std::size_t length);

    // insert <string, integer>
    template <std::size_t I>
    Cbore& item(const char (&key)[I], int32_t value)
    {
        if ((itemSize(I) + I + itemSize(value)) <= (maxLength - currentLength))
        {
            writeTypeAndValue(CborBase::TypeString, I - 1);
            writeBytes((const uint8_t*) key, I - 1);

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

    // insert <string, simple type>
    template <std::size_t I>
    Cbore& item(const char (&key)[I], CborBase::SimpleType_t value)
    {
        if ((itemSize(I) + I + 1) <= (maxLength - currentLength))
        {
            writeTypeAndValue(CborBase::TypeString, I - 1);
            writeBytes((const uint8_t*) key, I - 1);

            cbor[currentLength++] = CborBase::TypeSpecial << 5 | value;
        }

        return *this;
    }

    // insert <string, string>
    template <std::size_t I, std::size_t J>
    Cbore& item(const char (&key)[I], const char (&value)[J])
    {
        if ((itemSize(I) + itemSize(J) + I + J) <= (maxLength - currentLength))
        {
            writeTypeAndValue(CborBase::TypeString, I - 1);
            writeBytes((const uint8_t*) key, I - 1);

            writeTypeAndValue(CborBase::TypeString, J - 1);
            writeBytes((const uint8_t*) value, J - 1);
        }

        return *this;
    }

    // insert <string, string, length>
    template <std::size_t I>
    Cbore& item(const char (&key)[I], char* value, std::size_t length)
    {
        if ((itemSize(I) + itemSize(length) + I + length) <= (maxLength - currentLength))
        {
            writeTypeAndValue(CborBase::TypeString, I - 1);
            writeBytes((const uint8_t*) key, I - 1);

            writeTypeAndValue(CborBase::TypeString, length);
            writeBytes((const uint8_t*) value, length);
        }

        return *this;
    }

    /*************************************************************************/
    /* Complex map insertion                                                 */
    /*************************************************************************/

    Cbore& item(char* key, std::size_t keyLength, int32_t value);
    Cbore& item(char* key, std::size_t keyLength, CborBase::SimpleType_t value);
    Cbore& item(char* key, std::size_t keyLength, char* value, std::size_t valueLength);

    template <std::size_t I>
    Cbore& item(char* key, std::size_t keyLength, const char (&value)[I])
    {
        if ((itemSize(keyLength) + keyLength + itemSize(I) + I) <= (maxLength - currentLength))
        {
            writeTypeAndValue(CborBase::TypeString, keyLength);
            writeBytes((const uint8_t*) key, keyLength);

            writeTypeAndValue(CborBase::TypeString, I - 1);
            writeBytes((const uint8_t*) value, I - 1);
        }

        return *this;
    }

    /* debug */
    void print();

private:
    uint8_t itemSize(int32_t item);
    uint8_t writeTypeAndValue(CborBase::MajorType_t majorType, uint32_t value);
    uint32_t writeBytes(const uint8_t* source, uint32_t length);

private:
    uint8_t* cbor;
    std::size_t currentLength;
    std::size_t maxLength;
};

#endif // __CBORE_H__
