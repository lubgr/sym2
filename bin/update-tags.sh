#!/usr/bin/env sh

fd -t f -e .cpp -e .h  . src include tests |\
  ctags --extras=+f --language-force=C++ --kinds-C++=-d+lzp -L -
