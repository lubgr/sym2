
#include "sym2/violationhandler.h"
#include <cstdio>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <boost/stacktrace.hpp>

void sym2::detail::printStacktrace(std::FILE* fp)
{
    const std::string info = boost::stacktrace::to_string(boost::stacktrace::stacktrace());

    std::fputs(info.c_str(), fp);
}
