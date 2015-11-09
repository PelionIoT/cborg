# cborg
Resistance is voltage divided by current.

A simple CBOR encoder/decoder with a fluent API.

**Example**

Encode:

```C
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
          .key("something").value(str, sizeof(str) - 1)
          .key(str, sizeof(str) - 1).value(str, sizeof(str) - 1)
    .end()
  .item(-10)
  .item(10)
  .item(Cbor::TypeTrue)
.end();

encoder.print();
```
  
Decode:

```C
Cborg top(somebuffer, sizeof(somebuffer));
// find 'intents' array entry 2
Cborg intent = top.find("body").find("intents").at(2);
```

## License
This project is licensed under Apache-2.0

## Contributions / Pull Requests
All contributions are Apache 2.0. Only submit contributions where you have authored all of the code. If you do this on work time make sure your employer is cool with this.
