#!/usr/bin/env bash
# Compile and run a single exercise with ASan + UBSan.
# Usage: ./build.sh week1-cpp03-to-move/s01-object-lifetime/main.cpp
# -pedantic-errors: code the standard calls ill-formed is always an error,
# e.g. g++ otherwise only warns on narrowing from a variable inside {}.
# -Werror=vla: clang treats variable length arrays as a mere warning otherwise.
set -euo pipefail

if [ $# -lt 1 ]; then
    echo "Usage: $0 <path-to-main.cpp> [extra g++ args...]" >&2
    exit 1
fi

src="$1"
shift
out="$(mktemp -u /tmp/mcpp-XXXXXX)"

g++ -std=c++17 -Wall -Wextra -Wshadow -pedantic-errors -Werror=vla -g -O0 \
    -fsanitize=address,undefined -fno-omit-frame-pointer \
    "$src" -o "$out" "$@"

"$out"
rm -f "$out"
