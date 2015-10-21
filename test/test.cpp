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


#include "cborg/Cbor.h"

#include <stdio.h>
#include <string>

/*
    https://geraintluff.github.io/cbor-debug/

    D94011A366737461747573006269641941A764626F6479A2646E616D6572416E64793F73204D6163
    426F6F6B2041697267696E74656E747385D9400DA26269646F636F6D2E61726D2E6F627365727665
    68656E64706F696E74D820702F396234663264366431326435386537D9400DA262696473636F6D2E
    61726D2E656E766F792E626164676568656E64706F696E74D820712F643739383935313163663330
    39343437D9400DA262696475636F6D2E61726D2E6465766963652E756E6C6F636B68656E64706F69
    6E74D820712F62363237376534663231613034353164D9400DA26269646C636F6D2E61726D2E7069
    6E6768656E64706F696E74D820712F31383434393331363538643037633932D9400DA26269647819
    636F6D2E61726D2E636F6E6E65637469766974792E7769666968656E64706F696E74D820712F3132
    3261633336363164663231626637
*/

const uint8_t buffer[] = { 0xD9, 0x40, 0x11, 0xA3, 0x66, 0x73, 0x74, 0x61, 0x74, 0x75,
                  0x73, 0x00, 0x62, 0x69, 0x64, 0x19, 0x41, 0xA7, 0x64, 0x62,
                  0x6F, 0x64, 0x79, 0xA2, 0x64, 0x6E, 0x61, 0x6D, 0x65, 0x72,
                  0x41, 0x6E, 0x64, 0x79, 0x3F, 0x73, 0x20, 0x4D, 0x61, 0x63,
                  0x42, 0x6F, 0x6F, 0x6B, 0x20, 0x41, 0x69, 0x72, 0x67, 0x69,
                  0x6E, 0x74, 0x65, 0x6E, 0x74, 0x73, 0x85, 0xD9, 0x40, 0x0D,
                  0xA2, 0x62, 0x69, 0x64, 0x6F, 0x63, 0x6F, 0x6D, 0x2E, 0x61,
                  0x72, 0x6D, 0x2E, 0x6F, 0x62, 0x73, 0x65, 0x72, 0x76, 0x65,
                  0x68, 0x65, 0x6E, 0x64, 0x70, 0x6F, 0x69, 0x6E, 0x74, 0xD8,
                  0x20, 0x70, 0x2F, 0x39, 0x62, 0x34, 0x66, 0x32, 0x64, 0x36,
                  0x64, 0x31, 0x32, 0x64, 0x35, 0x38, 0x65, 0x37, 0xD9, 0x40,
                  0x0D, 0xA2, 0x62, 0x69, 0x64, 0x73, 0x63, 0x6F, 0x6D, 0x2E,
                  0x61, 0x72, 0x6D, 0x2E, 0x65, 0x6E, 0x76, 0x6F, 0x79, 0x2E,
                  0x62, 0x61, 0x64, 0x67, 0x65, 0x68, 0x65, 0x6E, 0x64, 0x70,
                  0x6F, 0x69, 0x6E, 0x74, 0xD8, 0x20, 0x71, 0x2F, 0x64, 0x37,
                  0x39, 0x38, 0x39, 0x35, 0x31, 0x31, 0x63, 0x66, 0x33, 0x30,
                  0x39, 0x34, 0x34, 0x37, 0xD9, 0x40, 0x0D, 0xA2, 0x62, 0x69,
                  0x64, 0x75, 0x63, 0x6F, 0x6D, 0x2E, 0x61, 0x72, 0x6D, 0x2E,
                  0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2E, 0x75, 0x6E, 0x6C,
                  0x6F, 0x63, 0x6B, 0x68, 0x65, 0x6E, 0x64, 0x70, 0x6F, 0x69,
                  0x6E, 0x74, 0xD8, 0x20, 0x71, 0x2F, 0x62, 0x36, 0x32, 0x37,
                  0x37, 0x65, 0x34, 0x66, 0x32, 0x31, 0x61, 0x30, 0x34, 0x35,
                  0x31, 0x64, 0xD9, 0x40, 0x0D, 0xA2, 0x62, 0x69, 0x64, 0x6C,
                  0x63, 0x6F, 0x6D, 0x2E, 0x61, 0x72, 0x6D, 0x2E, 0x70, 0x69,
                  0x6E, 0x67, 0x68, 0x65, 0x6E, 0x64, 0x70, 0x6F, 0x69, 0x6E,
                  0x74, 0xD8, 0x20, 0x71, 0x2F, 0x31, 0x38, 0x34, 0x34, 0x39,
                  0x33, 0x31, 0x36, 0x35, 0x38, 0x64, 0x30, 0x37, 0x63, 0x39,
                  0x32, 0xD9, 0x40, 0x0D, 0xA2, 0x62, 0x69, 0x64, 0x78, 0x19,
                  0x63, 0x6F, 0x6D, 0x2E, 0x61, 0x72, 0x6D, 0x2E, 0x63, 0x6F,
                  0x6E, 0x6E, 0x65, 0x63, 0x74, 0x69, 0x76, 0x69, 0x74, 0x79,
                  0x2E, 0x77, 0x69, 0x66, 0x69, 0x68, 0x65, 0x6E, 0x64, 0x70,
                  0x6F, 0x69, 0x6E, 0x74, 0xD8, 0x20, 0x71, 0x2F, 0x31, 0x32,
                  0x32, 0x61, 0x63, 0x33, 0x36, 0x36, 0x31, 0x64, 0x66, 0x32,
                  0x31, 0x62, 0x66, 0x37
                };


/*
    Print CBOR encoding of object.
*/
void printfCborg(Cborg& object)
{
    const uint8_t* pointer = NULL;
    uint32_t length = 0;

    bool result = object.getCBOR(&pointer, &length);

    if (result)
    {
        for (std::size_t idx = 0; idx < length; idx++)
        {
            printf("%02X", pointer[idx]);
        }
        printf("\r\n");
    }
    else
    {
        printf("error\r\n");
    }
}


/*
    Test 1: full decode of CBOR object and print to stdout.
*/
void test1()
{
    Cborg top(buffer, sizeof(buffer));

    printf("Test 1: Decode CBOR object:\r\n");

    top.print();

    printf("\r\n===============================================================================\r\n");
}


/*
    Test 2: map and array operations.
*/
void test2()
{
    Cborg top(buffer, sizeof(buffer));

    printf("Test 2: All endpoints in CBOR object:\r\n");

    // find intents array
    Cborg intents = top.find("body").find("intents");

    // loop through all intents and print endpoints
    for (std::size_t idx = 0; idx < intents.getSize(); idx++)
    {
        Cborg endpoint = intents.at(idx).find("endpoint");

        endpoint.print();
    }

    printf("\r\n===============================================================================\r\n");
}

/*
    Test 3: retrieve CBOR encoding from nested objects.
*/
void test3()
{
    Cborg top(buffer, sizeof(buffer));

    printf("Test 3: Raw CBOR encoding of intent at index 2:\r\n");

    // find intents array entry 2
    Cborg intent = top.find("body").find("intents").at(2);

    printfCborg(intent);

    printf("\r\n===============================================================================\r\n");
}


/*
    Test 4: retrieve non-container values.
*/
void test4()
{
    Cborg top(buffer, sizeof(buffer));

    printf("Test 4: Non-container types:\r\n");

    // integer
    {
        Cborg object =  top.find("id");

        uint32_t integer = 0;

        if (object.getType() == Cbor::TypeUnsigned)
        {
            bool result = object.getUnsigned(&integer);

            if (result)
            {
                printf("Unsigned integer: %u\r\n", integer);
            }
        }
    }

    // string
    {
        Cborg object =  top.find("body").find("name");

        const char* name = NULL;
        uint32_t length;

        if (object.getType() == Cbor::TypeString)
        {
            bool restult = object.getString(&name, &length);

            if (restult)
            {
                std::string objectString(name, length);

                printf("String (pointer, length): %s\r\n", objectString.c_str());
            }
        }
    }

    // string
    {
        Cborg object =  top.find("body").find("name");

        const char* name = NULL;
        uint32_t length;

        if (object.getType() == Cbor::TypeString)
        {
            std::string objectString;
            bool restult = object.getString(objectString);

            if (restult)
            {
                printf("String (std::string): %s\r\n", objectString.c_str());
            }
        }
    }

    printf("\r\n===============================================================================\r\n");
}


/*
    Test primitives
*/
void test5()
{
    printf("Test 5: Encode primitives\r\n");

    uint8_t buffer[100];

    /* Test NULL */
    {
        printf("CBOR NULL: ");

        Cbor cbornull;

        uint32_t written = cbornull.writeCBOR(buffer, sizeof(buffer));

        for (std::size_t idx = 0; idx < written; idx++)
        {
            printf("%02X", buffer[idx]);
        }
        printf("\r\n");

        // cross check
        printf("Cross check: ");
        Cborg borg(buffer, sizeof(buffer));
        borg.print();
        printf("\r\n");
    }

    /* Test true */
    {
        printf("CBOR true: ");

        CborBool cborbool(true);

        uint32_t written = cborbool.writeCBOR(buffer, sizeof(buffer));

        for (std::size_t idx = 0; idx < written; idx++)
        {
            printf("%02X", buffer[idx]);
        }
        printf("\r\n");

        // cross check
        printf("Cross check: ");
        Cborg borg(buffer, sizeof(buffer));
        borg.print();
        printf("\r\n");
    }

    /* Test false */
    {
        printf("CBOR false: ");

        CborBool cborbool(false);

        uint32_t written = cborbool.writeCBOR(buffer, sizeof(buffer));

        for (std::size_t idx = 0; idx < written; idx++)
        {
            printf("%02X", buffer[idx]);
        }
        printf("\r\n");

        // cross check
        printf("Cross check: ");
        Cborg borg(buffer, sizeof(buffer));
        borg.print();
        printf("\r\n");
    }

    /* Test string from const string */
    {
        printf("CBOR const string with tag [1234][Attention, the Universe!]: ");

        CborString cborstr("Attention, the Universe!");

        cborstr.setTag(1234);

        uint32_t written = cborstr.writeCBOR(buffer, sizeof(buffer));

        for (std::size_t idx = 0; idx < written; idx++)
        {
            printf("%02X", buffer[idx]);
        }
        printf("\r\n");

        // cross check
        printf("Cross check: ");
        Cborg borg(buffer, sizeof(buffer));
        borg.print();
        printf("\r\n");
    }

    /* Test string from char array with length */
    {
        printf("CBOR char array with tag [1234][Hello World!]: ");

        const char str[] = "Hello World!";
        CborString cborstr(str, sizeof(str) - 1);
        cborstr.setTag(1234);

        uint8_t buffer[100];
        std::size_t index = 0;

        uint32_t written = cborstr.writeCBOR(buffer, sizeof(buffer));

        for (std::size_t idx = 0; idx < written; idx++)
        {
            printf("%02X", buffer[idx]);
        }
        printf("\r\n");

        // cross check
        printf("Cross check: ");
        Cborg borg(buffer, sizeof(buffer));
        borg.print();
        printf("\r\n");
    }

    /* Test byte array from array with length */
    {
        printf("CBOR byte array with tag [1234]{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF }: ");

        const uint8_t bytes[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
        CborBytes cborbytes(bytes, sizeof(bytes));
        cborbytes.setTag(1234);

        uint32_t written = cborbytes.writeCBOR(buffer, sizeof(buffer));

        for (std::size_t idx = 0; idx < written; idx++)
        {
            printf("%02X", buffer[idx]);
        }
        printf("\r\n");

        // cross check
        printf("Cross check: ");
        Cborg borg(buffer, sizeof(buffer));
        borg.print();
        printf("\r\n");
    }

    /* Test integers */
    {
        printf("CBOR positive integer [1234567890]: ");

        uint32_t positive = 0x499602D2;

        CborInteger cborpos(positive);

        uint32_t written = cborpos.writeCBOR(buffer, sizeof(buffer));

        for (std::size_t idx = 0; idx < written; idx++)
        {
            printf("%02X", buffer[idx]);
        }
        printf("\r\n");

        // cross check
        printf("Cross check: ");
        Cborg borg(buffer, sizeof(buffer));
        borg.print();
        printf("\r\n");
    }

    {
        printf("CBOR negative integer [-1234567890]: ");

        int32_t negative = -1234567890;

        CborInteger cborneg(negative);

        uint32_t written = cborneg.writeCBOR(buffer, sizeof(buffer));

        for (std::size_t idx = 0; idx < written; idx++)
        {
            printf("%02X", buffer[idx]);
        }
        printf("\r\n");

        // cross check
        printf("Cross check: ");
        Cborg borg(buffer, sizeof(buffer));
        borg.print();
        printf("\r\n");
    }

    printf("\r\n===============================================================================\r\n");
}

/*
    Test container classes
*/
void test6()
{
    printf("Test 6: Encode static container classes\r\n");

    uint8_t buffer[200];

    {
        // top array
        CborArrayStatic<8> array;

        CborString str1("string 1");
        CborString str2("string 2");
        str1.setTag(1);
        str2.setTag(2);
        array.insert(str1);
        array.insert(str2);

            // sub array
            CborArrayStatic<3> subarray;
            subarray.setTag(3);
            CborString substr1("substring 1");
            CborString substr2("substring 2");
            CborString substr3("substring 3");
            substr1.setTag(31);
            substr2.setTag(32);
            substr3.setTag(33);
            subarray.insert(substr1);
            subarray.insert(substr2);
            subarray.insert(substr3);

        array.insert(subarray);

        CborString str3("string 3");
        str3.setTag(4);
        array.insert(str3);

            // sub map
            CborMapStatic<3> submap;
            submap.setTag(5);
            CborString key1("key1");
            CborString value1("value1");
            CborInteger key2(7);
            CborInteger value2(-8);

            value1.setTag(51);
            value2.setTag(52);

            submap.insert(key1, value1);
            submap.insert(&key2, &value2);

                CborString key3("key3");
                CborArrayStatic<3> value3;
                value3.setTag(53);
                CborString subvalue1("subvalue1");
                CborString subvalue2("subvalue2");
                CborString subvalue3("eight nine");
                subvalue1.setTag(531);
                subvalue2.setTag(532);
                value3.insert(&subvalue1);
                value3.insert(&subvalue2);
                value3.insert(subvalue3);

            submap.insert(key3, value3);

        array.insert(submap);

        CborInteger int1(5);
        CborInteger int2(-5);
        CborInteger int3(1234);
        int1.setTag(6);
        int2.setTag(7);

        array.insert(int1);
        array.insert(int2);
        array.insert(int3);

        printf("Test debug print\r\n");
        array.print();

        printf("Test CBOR encoding\r\n");
        uint32_t written = array.writeCBOR(buffer, sizeof(buffer));

        for (std::size_t idx = 0; idx < written; idx++)
        {
            printf("%02X", buffer[idx]);
        }
        printf("\r\n");

        // cross check
        printf("Test CBOR decoding and print out\r\n");
        Cborg borg(buffer, sizeof(buffer));
        borg.print();
        printf("\r\n");

    }
}

/*
    Test container classes
*/
void test7()
{
    printf("Test 6: Encode dynamic container classes\r\n");

    uint8_t buffer[200];

    {
        // top array
        CborArray array;

        array.insert("string 1");
        array.insert("string 2");

            // sub array
            CborArray subarray;
            subarray.setTag(3);
            subarray.insert("substring 1");
            subarray.insert("substring 2");
            subarray.insert("substring 3");

        array.insert(subarray);
        array.insert("string 3");

            // sub map
            CborMap submap;
            submap.setTag(5);
            CborString value1("value1");
            CborInteger value2(-8);

            value1.setTag(51);
            value2.setTag(52);

            submap.insert("key1", value1);
            submap.insert(7, &value2);

                CborArray value3;
                value3.setTag(53);
                value3.insert("subvalue1");
                value3.insert("subvalue2");
                value3.insert("eight nine");

            submap.insert("key3", value3);

        array.insert(submap);
        array.insert(5);
        array.insert(-5);
        array.insert(1234);

        printf("Test debug print\r\n");
        array.print();

        printf("Test CBOR encoding\r\n");
        uint32_t written = array.writeCBOR(buffer, sizeof(buffer));

        for (std::size_t idx = 0; idx < written; idx++)
        {
            printf("%02X", buffer[idx]);
        }
        printf("\r\n");

        // cross check
        printf("Test CBOR decoding and print out\r\n");
        Cborg borg(buffer, sizeof(buffer));
        borg.print();
        printf("\r\n");
    }
}

void test8()
{
    uint8_t buffer[200];

    char str[] = "hello";

    Cbore encoder(buffer, sizeof(buffer));

    encoder.tag(1234)
            .array()
                .item("Attention, the Universe!")
                .item("Hello World!")
                .map()
                    .key("key1").value("value1")
                    .key("key2")
                        .array()
                            .item("one")
                            .item("two")
                            .item("three")
                        .end()
                    .key(3).value("value3")
                    .key("key4")
                        .map(4)
                            .key("key5").value(Cbor::TypeNull)
                            .key(6).value(Cbor::TypeFalse)
                            .key(7)
                                .array(3)
                                    .item(1)
                                    .item(str, sizeof(str) - 1)
                                    .item(3)
//                                .end()
                            .key("something").value(str, sizeof(str) - 1)
//                        .end()
                    .key(str, sizeof(str) - 1).value(str, sizeof(str) - 1)
                .end()
                .item(-10)
                .item(10)
                .item(Cbor::TypeTrue)
            .end();

    encoder.print();

    for (std::size_t idx = 0; idx < encoder.getLength(); idx++)
    {
        printf("%02X", buffer[idx]);
    }
    printf("\r\n");

    // cross check
    printf("Test CBOR decoding and print out\r\n");
    Cborg decoder(buffer, sizeof(buffer));

    decoder.at(2).find("key4").find(7).print();

    printf("\r\n");
}

void test9()
{
    const uint8_t stream[] = { 0x7f, 0x65, 0x73, 0x74, 0x72, 0x65, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x67, 0xff };

    Cborg top(stream, sizeof(stream));

    top.print();
}



/*****************************************************************************/
/* App start                                                                 */
/*****************************************************************************/
void app_start(int, char *[])
{
    test1();
    test2();
    test3();
    test4();

    test5();
    test6();
    test7();

    test8();
    test9();
}

/*****************************************************************************/
/* Compatibility                                                             */
/*****************************************************************************/

#if defined(YOTTA_MINAR_VERSION_STRING)
/*********************************************************/
/* Build for mbed OS                                     */
/*********************************************************/

#else
/*********************************************************/
/* Build for mbed Classic                                */
/*********************************************************/
int main(void)
{
    app_start(0, NULL);
}
#endif
