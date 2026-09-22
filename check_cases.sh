#!/usr/bin/env bash
# Proverava POGREŠNE primere jedne lekcije, sa g++ i clang++ (šta je instalirano).
#   <lekcija>/errors/*.cpp  -- mora da PADNE pri kompajliranju, sa porukom koja
#                              sadrži "// EXPECT-GCC:" / "// EXPECT-CLANG:" tekst
#   <lekcija>/ub/*.cpp      -- mora da se kompajlira, a ASan/UBSan pri
#                              pokretanju mora da prijavi "// EXPECT-UB:" (regex)
# Opciono "// STD: c++20" u fajlu bira standard (podrazumevano c++17).
# Opciono "// LINK: support/a.cpp support/b.cpp" (putanje relativno od fajla):
#   fajl se kompajlira ZAJEDNO sa tim fajlovima i LINKUJE, pa greška sme da
#   bude i od linkera ("multiple definition", "undefined reference").
# Usage: ./check_cases.sh week0-fundamentals/04-pointers-and-references
set -u

if [ $# -ne 1 ] || [ ! -d "$1" ]; then
    echo "Usage: $0 <folder lekcije>" >&2
    exit 2
fi
lesson="$1"
fail=0
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT

compilers=()
for cc in g++ clang++; do
    command -v "$cc" >/dev/null 2>&1 && compilers+=("$cc")
done

strict_flags() { # isti strogi flegovi kao build.sh / build.ps1
    local flags=(-std="$2" -Wall -Wextra -Wshadow -pedantic-errors -Werror=vla)
    [ "$1" = "clang++" ] && flags+=(-Werror=reorder-init-list)
    printf '%s\n' "${flags[@]}"
}

header() { sed -n "s|^// $1: *||p" "$2"; }

link_files() { # "// LINK:" fajlovi, sa putanjom relativnom od fajla
    local x
    for x in $(header LINK "$1"); do printf '%s\n' "$(dirname "$1")/$x"; done
}

for f in "$lesson"/errors/*.cpp; do
    [ -e "$f" ] || continue
    name="errors/$(basename "$f")"
    std="$(header STD "$f")"; std="${std:-c++17}"
    for cc in "${compilers[@]}"; do
        key=GCC; [ "$cc" = "clang++" ] && key=CLANG
        expect="$(header "EXPECT-$key" "$f")"
        mapfile -t flags < <(strict_flags "$cc" "$std")
        mapfile -t extra < <(link_files "$f")
        if [ "${#extra[@]}" -gt 0 ]; then
            mode=(-o "$tmp/link" "${extra[@]}")   # kompajliraj i linkuj
        else
            mode=(-fsyntax-only)
        fi
        if out="$("$cc" "${flags[@]}" "${mode[@]}" "$f" 2>&1)"; then
            echo "FAIL  $name [$cc]: kompajliralo se, a ne bi smelo"; fail=1
        elif ! grep -qF -- "$expect" <<<"$out"; then
            echo "FAIL  $name [$cc]: pada, ali ne iz očekivanog razloga (\"$expect\"):"
            grep -m2 -E 'error|multiple definition|undefined reference' <<<"$out"; fail=1
        else
            echo "ok    $name [$cc]"
        fi
    done
done

for f in "$lesson"/ub/*.cpp; do
    [ -e "$f" ] || continue
    name="ub/$(basename "$f")"
    std="$(header STD "$f")"; std="${std:-c++17}"
    expect="$(header EXPECT-UB "$f")"
    for cc in "${compilers[@]}"; do
        # Sanitizer runtime nije uvek instaliran za svaki kompajler -- proveri.
        if ! echo 'int main(){}' | "$cc" -x c++ -fsanitize=address,undefined - -o "$tmp/probe" >/dev/null 2>&1; then
            echo "skip  $name [$cc]: nema ASan/UBSan runtime za ovaj kompajler"
            continue
        fi
        mapfile -t flags < <(strict_flags "$cc" "$std")
        mapfile -t extra < <(link_files "$f")
        if ! "$cc" "${flags[@]}" -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer \
                "$f" "${extra[@]}" -o "$tmp/ub" 2>"$tmp/cc.log"; then
            echo "FAIL  $name [$cc]: ne kompajlira se, a trebalo bi"
            grep -m2 'error' "$tmp/cc.log"; fail=1
            continue
        fi
        out="$("$tmp/ub" 2>&1)"
        if grep -qE -- "$expect" <<<"$out"; then
            echo "ok    $name [$cc]"
        else
            echo "FAIL  $name [$cc]: sanitizer nije prijavio \"$expect\""; fail=1
        fi
    done
done

exit "$fail"
