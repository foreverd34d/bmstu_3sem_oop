cmake_minimum_required(VERSION 3.17)

project(mylist CXX)
set(TESTS_NAME ${PROJECT_NAME}_tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS " -Wall -Wpedantic -Wextra -Wfloat-conversion -Wfloat-equal -Wvla")

find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)

add_executable(${TESTS_NAME}
    tests/list.test.cpp
)

target_include_directories(${TESTS_NAME} PRIVATE include)
target_link_libraries(${TESTS_NAME} PRIVATE Catch2::Catch2WithMain)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fsanitize=address"
    )
    target_link_options(${PROJECT_NAME}
        BEFORE PUBLIC -fsanitize=undefined PUBLIC -fsanitize=address
    )
endif()
