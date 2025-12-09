cmake_minimum_required(VERSION 3.14)
project(BitArray LANGUAGES CXX)

# Требуем C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Автоматическая загрузка Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Исходники BitArray
set(BITARRAY_SOURCES bitarray.cpp bitarray.h)

# Статическая библиотека
add_library(bitarray STATIC ${BITARRAY_SOURCES})
target_include_directories(bitarray PUBLIC .)

# Демо-программа
add_executable(main main.cpp)
target_link_libraries(main bitarray)

# Юнит-тесты
add_executable(bitarray_tests bitarray_tests.cpp)
target_link_libraries(bitarray_tests bitarray GTest::gtest GTest::gtest_main)

# Поддержка CTest
enable_testing()
add_test(NAME BitArrayTests COMMAND bitarray_tests)
