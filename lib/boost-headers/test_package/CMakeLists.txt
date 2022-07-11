
cmake_minimum_required(VERSION 3.20)
project(package-test)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(Boost_INCLUDE_DIRS ${CONAN_INCLUDE_DIRS_BOOST-HEADERS})

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${Boost_INCLUDE_DIRS})
