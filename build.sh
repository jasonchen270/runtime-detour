#!/bin/sh
# Build the detour demo (Linux, x86-64 or arm64).
set -e
cd "$(dirname "$0")"
"${CXX:-c++}" -std=c++17 -O2 detour.cpp -o detour
echo "built ./detour"
