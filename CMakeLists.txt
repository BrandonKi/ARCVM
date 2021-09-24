cmake_minimum_required(VERSION 3.0.0)
project(ARCVM VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 20)

cmake_policy(SET CMP0069 NEW)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_library(arcvm_lib OBJECT 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Arcvm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/IRGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/IRPrinter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/IRInterpreter.cpp
)

target_include_directories(arcvm_lib
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    $<TARGET_OBJECTS:arcvm_lib>
)

add_executable(ARCVM ${SRC})

target_compile_definitions(ARCVM PRIVATE DEBUG_BUILD)

target_include_directories(ARCVM 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# hmmm keep an eye on this
if(${LTO})
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result)
    if(result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

if(MSVC)
    target_compile_options(ARCVM PRIVATE 
        /W3 
        # /Wall 
        # # # C5045 Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
        # /wd5045
        # # # C5039 'function': pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior may occur if this function throws an exception.
        # /wd5039
        # # # C4668 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
        # /wd4668

        # /wd5027
        # /wd4626
        # /wd4820
        # /wd4061
        # /wd4062
        # /wd4623
    )
else()
    target_compile_options(ARCVM PRIVATE -Wall -Wextra -pedantic)
endif()

set(TEST_SRC
    $<TARGET_OBJECTS:arcvm_lib>
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/run_tests.cpp
)

add_executable(tests ${TEST_SRC})

target_include_directories(tests 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib
)