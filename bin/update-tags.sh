#!/usr/bin/env sh

fd -t f -e .cpp -e .cpp -e .h |\
  /usr/local/bin/ctags --extras=+f --language-force=C++ --kinds-C++=-d+lzp -L -
