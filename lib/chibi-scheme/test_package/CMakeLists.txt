
cmake_minimum_required(VERSION 3.20)
project(package-test)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(${PROJECT_NAME} main.cpp)
conan_target_link_libraries(${PROJECT_NAME})
