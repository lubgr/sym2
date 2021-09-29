
#include "violationhandler.h"
#include <cstdio>

#define _GNU_SOURCE
#include <boost/stacktrace.hpp>

void sym2::detail::printStacktrace(std::FILE* fp)
{
    const std::string info = boost::stacktrace::to_string(boost::stacktrace::stacktrace());

    std::fputs(info.c_str(), fp);
}
