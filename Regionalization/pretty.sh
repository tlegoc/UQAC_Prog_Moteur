#!/usr/bin/env sh
find examples include lib src \( -name "*.h" -o -name "*.cpp" -o -name "*.inl" \) -exec clang-format -i {} \;
