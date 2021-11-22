add_library(cborg-lib STATIC
        source/CborBase.cpp
        source/Cbore.cpp
        source/Cborg.cpp
        )

target_include_directories(cborg-lib PUBLIC include/)

if (CMAKE_BUILD_TYPE MATCHES "Debug")

    add_executable(cborg-test
            test/test.cpp)

    target_link_libraries(cborg-test PUBLIC
            cborg-lib)

endif ()