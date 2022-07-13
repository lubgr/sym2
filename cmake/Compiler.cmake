

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11)
    message(WARNING "Gcc 11 is required to compile this project. Expect a build failure")
endif()
