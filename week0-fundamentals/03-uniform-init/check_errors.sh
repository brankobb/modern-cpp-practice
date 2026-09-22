#!/usr/bin/env bash
# Proverava da se SVAKI primer iz errors/ NE kompajlira, i to iz očekivanog
# razloga (poruka mora da sadrži tekst iz "// EXPECT-GCC:" / "// EXPECT-CLANG:").
# Radi sa g++ i clang++, šta god je instalirano.
# Usage: ./check_errors.sh
set -u

dir="$(cd "$(dirname "$0")" && pwd)"
fail=0
compilers=()
for cc in g++ clang++; do
    command -v "$cc" >/dev/null 2>&1 && compilers+=("$cc")
done

for f in "$dir"/errors/*.cpp; do
    name="$(basename "$f")"
    std="$(sed -n 's|^// STD: *||p' "$f")"
    std="${std:-c++17}"
    for cc in "${compilers[@]}"; do
        flags=(-std="$std" -Wall -Wextra -Wshadow -pedantic-errors -fsyntax-only)
        if [ "$cc" = "clang++" ]; then
            key=CLANG
            flags+=(-Werror=reorder-init-list)
        else
            key=GCC
        fi
        expect="$(sed -n "s|^// EXPECT-$key: *||p" "$f")"
        if out="$("$cc" "${flags[@]}" "$f" 2>&1)"; then
            echo "FAIL  $name [$cc]: kompajliralo se, a ne bi smelo"
            fail=1
        elif ! grep -qF -- "$expect" <<<"$out"; then
            echo "FAIL  $name [$cc]: greška postoji, ali nije očekivana (\"$expect\"):"
            grep -m2 'error' <<<"$out"
            fail=1
        else
            echo "ok    $name [$cc]"
        fi
    done
done

exit "$fail"
