
#include <cassert>
#include <cstdio>
#include <filesystem>
#include <linenoise.h>
#include <string>
#include <string_view>
#include "s7bridge.h"

struct LinenoiseHistory {
    std::string path;

    LinenoiseHistory(int length, std::string_view historyFile)
        : path{historyFile}
    {
        linenoiseHistorySetMaxLen(length);
        linenoiseHistoryLoad(path.c_str());
    }

    ~LinenoiseHistory()
    {
        linenoiseHistorySave(path.c_str());
    }
};

int main(int argc, char** argv)
{
    const LinenoiseHistory hist{500, ".s7history"};
    sym2::S7Interperter s7{};

    for (int i = 1; i < argc; ++i) {
        const char* path = argv[i];
        if (std::filesystem::is_regular_file(path))
            s7.loadFile(path);
        else
            std::fprintf(stderr, "Can't open %s\n", path);
    }

    char* input = nullptr;
    while ((input = linenoise("sym7> ")) != nullptr) {
        const auto result = s7.eval(input);

        std::puts(s7.toString(result).c_str());

        linenoiseHistoryAdd(input);
        linenoiseFree(input);
    }

    return 0;
}
