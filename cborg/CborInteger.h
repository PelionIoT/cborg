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

#ifndef __CBOR_INTEGER_H__
#define __CBOR_INTEGER_H__

#include "cborg/CborBase.h"

#include <stdint.h>
#include <cstddef>

#include <stdio.h>


class CborInteger : public CborBase
{
public:
    union DataUnion {
        uint32_t positive;
        int32_t negative;
    };

    CborInteger(uint32_t integer)
        :   CborBase(TypeUnsigned, TypeUndefined)
    {
        data.positive = integer;
    }

    CborInteger(int32_t integer)
        :   CborBase(TypeUnsigned, TypeUndefined)
    {
        if (integer < 0)
        {
            majorType = TypeNegative;
            data.negative = integer;
        }
        else
        {
            // majorType = TypeUnsigned;
            data.positive = integer;
        }
    }

    /*************************************************************************/
    /* Encode                                                                */
    /*************************************************************************/

    virtual uint32_t writeCBOR(uint8_t* destination, uint32_t maxLength)
    {
        uint32_t written = 0;

        if (destination)
        {
            // write tag if set
            if (tag != TypeUnassigned)
            {
                written += writeTypeAndValue(destination, maxLength, TypeTag, tag);
            }

            if (majorType == TypeUnsigned)
            {
                written += writeTypeAndValue(destination + written, maxLength - written, TypeUnsigned, data.positive);
            }
            else
            {
                written += writeTypeAndValue(destination + written, maxLength - written, TypeNegative, -1 - data.negative);
            }
        }

        return written;
    }

    /*************************************************************************/
    /* Debug                                                                 */
    /*************************************************************************/

    virtual void print()
    {
        // write tag if set
        if (tag != TypeUnassigned)
        {
            printf("[%lu] ", tag);
        }

        if (majorType == TypeUnsigned)
        {
            printf("%lu\r\n", data.positive);
        }
        else
        {
            printf("%ld\r\n", data.negative);
        }
    }

private:
    DataUnion data;
};


#endif // __CBOR_INTEGER_H__
