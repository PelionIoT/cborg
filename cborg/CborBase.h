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


#ifndef __CBOR_BASE_H__
#define __CBOR_BASE_H__

#include <stdint.h>
#include <cstddef>
#include <list>

#include <stdio.h>

class CborBase;
extern CborBase CborNull;

class CborBase
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
        TypeRaw         = 0xFE,
        TypeUnassigned  = 0xFF
    } MajorType_t;

    typedef enum {
        TypeFalse       = 0x14,
        TypeTrue        = 0x15,
        TypeNull        = 0x16,
        TypeUndefined   = 0x17,
        TypeUnknown     = 0x18,
        TypeHalfFloat   = 0x19,
        TypeSingleFloat = 0x1A,
        TypeDoubleFloat = 0x1B,
        TypeIndefinite  = 0x1F
    } SimpleType_t;

    CborBase(MajorType_t _majorType = TypeSpecial, SimpleType_t _minorType = TypeNull)
        :   majorType(_majorType),
            minorType(_minorType),
            tag(TypeUnassigned)
    {}

    virtual ~CborBase() {};

    /*************************************************************************/
    /* Encode                                                                */
    /*************************************************************************/

    virtual uint32_t writeCBOR(uint8_t* destination, uint32_t maxLength);

    /*************************************************************************/
    /* Mutators                                                              */
    /*************************************************************************/

    uint8_t getType() const
    {
        return majorType;
    }

    void setTag(uint32_t _tag)
    {
        tag = _tag;
    }

    uint32_t getTag() const
    {
        return tag;
    }

    /*************************************************************************/
    /* Container                                                             */
    /*************************************************************************/

    virtual uint32_t getLength() const
    {
        return 0;
    }

    virtual uint32_t getSize() const
    {
        return 0;
    }

    /*************************************************************************/
    /* Array                                                                 */
    /*************************************************************************/

    virtual CborBase* at(std::size_t index)
    {
        (void) index;

        return &CborNull;
    }

    /*************************************************************************/
    /* Map                                                                   */
    /*************************************************************************/

    virtual CborBase* key(std::size_t index)
    {
        (void) index;

        return &CborNull;
    }

    virtual CborBase* value(std::size_t index)
    {
        (void) index;

        return &CborNull;
    }

    /*************************************************************************/
    /* Debug                                                                 */
    /*************************************************************************/

    virtual void print();

protected:
    static uint32_t writeQueue(uint8_t* destination, uint32_t maxLength, std::list<CborBase*>& queue);
    static uint8_t writeTypeAndValue(uint8_t* destination, uint32_t maxLength, uint8_t majorType, uint32_t value);
    static uint32_t writeBytes(uint8_t* destination, uint32_t maxLength, const uint8_t* source, uint32_t length);
    static void printQueue(std::list<CborBase*> queue);

protected:
    uint8_t majorType;
    uint8_t minorType;
    uint32_t tag;
};

#endif // __CBOR_BASE_H__
