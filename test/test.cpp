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

    printf("\r\n===============================================================================\r\n");
}

void test6()
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

void test7()
{
    const uint8_t stream[] = { 0x7f, 0x65, 0x73, 0x74, 0x72, 0x65, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x67, 0xff };

    Cborg top(stream, sizeof(stream));

    top.print();
}

void test8()
{
    uint8_t buffer[32];
    Cbore encoder(buffer, 32);
    Cborg decoder(buffer, 32);

    printf("Test reset function, encoding 1 on new Cbore object\r\n");

    encoder.tag(1234).array().item(1).item(2).item(3).end();
    encoder.print();

    encoder.reset(false /* resetBuffer*/);

    printf("Test reset function, encoding 2 on same Cbore object\r\n");

    encoder.tag(1234).array().item(1).item(2).item(3).end();
    encoder.print();

    encoder.reset(true /* resetBuffer*/);

    printf("Test reset function, encoding 3 on same Cbore object with buffer reset\r\n");

    encoder.tag(1234).array().item(1).item(2).item(3).end();
    encoder.print();
}

void test9()
{
    Cborg top(buffer, sizeof(buffer));

    printf("Test 9: Iterate map.\r\n");

    Cborg key = top.getKey(0);
    std::string keyName;
    key.getString(keyName);
    Cborg val = top.find(keyName.c_str(), keyName.size());
    uint32_t value;
    val.getUnsigned(&value);
    printf("First key: %s value: %d\r\n", keyName.c_str(), value);

    key = top.getKey(1);
    key.getString(keyName);
    val = top.find(keyName.c_str(), keyName.size());
    val.getUnsigned(&value);
    printf("Second key: %s value: %d\r\n", keyName.c_str(), value);
}

void test10()
{
    uint8_t buffer[317];
    Cbore encoder(buffer, 317);

    encoder
        .map(1)
            .key("actions")
            .array(1)
                .map()
                    .key("action").value("c2pa.repackaged")
                    .key("when").tag(0).value("2022-11-18T19:47:11Z")
                    .key("softwareAgent")
                        .map(2)
                            .key("name").value("Truepic libc2pa C++ Library")
                            .key("version").value("3.1.28")
                    .key("parameters")
                        .map(1)
                            .key("ingredients")
                                .array(1)
                                    .map(3)
                                        .key("alg").value("sha256")
                                        .key("hash").value("the_hash")
                                        .key("url").value("the_url")
                .end();

    Cborg top(buffer, sizeof(buffer));

    printf("Test 10: Iterate past map in map.\r\n");

    Cborg map = top.find("actions").at(0);
    Cborg key = map.getKey(3);
    std::string keyName;
    key.getString(keyName);
    printf("Fourth key: %s\r\n", keyName.c_str());
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
    test10();
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
