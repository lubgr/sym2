#!/usr/bin/env sh

# Requires universal ctags (http://ctags.io), not the oder excuberant ctags
fd -t f -e cc -e cxx -e cpp -e h -e hpp |\
  ctags --extras=+f --language-force=C++ --kinds-C++=-d+lzp -L -
