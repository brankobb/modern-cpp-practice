#!/usr/bin/env bash
# Proverava POGREŠNE primere jedne lekcije, sa g++ i clang++ (šta je instalirano).
#   <lekcija>/errors/*.cpp  -- mora da PADNE pri kompajliranju, sa porukom koja
#                              sadrži "// EXPECT-GCC:" / "// EXPECT-CLANG:" tekst
#   <lekcija>/ub/*.cpp      -- mora da se kompajlira, a ASan/UBSan pri
#                              pokretanju mora da prijavi "// EXPECT-UB:" (regex)
#   <lekcija>/runtime/*.cpp -- nije UB, ali program se prekine pri izvršavanju
#                              (npr. std::terminate): mora da se kompajlira,
#                              izađe sa greškom i ispiše "// EXPECT-RUN:" (regex);
#                              bez sanitizera, pa se proverava i sa clang++
# Opciono "// STD: c++20" u fajlu bira standard (podrazumevano c++17).
# Opciono "// LINK: support/a.cpp support/b.cpp" (putanje relativno od fajla):
#   fajl se kompajlira ZAJEDNO sa tim fajlovima i LINKUJE, pa greška sme da
#   bude i od linkera ("multiple definition", "undefined reference").
# Opciono "// ONLY-CC: g++": slučaj važi samo za taj kompajler (npr. UB koji
#   drugi kompajler odbije već pri kompajliranju); ostali se preskaču.
# Opciono "// FLAGS: -fsanitize=float-cast-overflow": dodatni flegovi za taj
#   fajl (npr. sanitizer koji -fsanitize=undefined ne uključuje).
# Opciono "// SANITIZER: thread" u ub/ fajlu: ThreadSanitizer umesto
#   ASan/UBSan (data race, redosled zaključavanja); ne mogu zajedno.
# Usage: ./check_cases.sh 1-language-basics/04-pointers-and-references
set -u

if [ $# -ne 1 ] || [ ! -d "$1" ]; then
    echo "Usage: $0 <lesson folder>" >&2
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

tbb_libs() { # cc fajl -> -ltbb ako fajl koristi <execution>, a libstdc++ izabere TBB
    grep -q '#include <execution>' "$2" &&
        echo '#include <execution>' | "$1" -std=c++17 -x c++ -dM -E - 2>/dev/null | grep -q _PSTL_PAR_BACKEND_TBB &&
        echo -ltbb
    return 0
}

link_files() { # "// LINK:" fajlovi, sa putanjom relativnom od fajla
    local x
    for x in $(header LINK "$1"); do printf '%s\n' "$(dirname "$1")/$x"; done
}

for f in "$lesson"/errors/*.cpp; do
    [ -e "$f" ] || continue
    name="errors/$(basename "$f")"
    std="$(header STD "$f")"; std="${std:-c++17}"
    only="$(header ONLY-CC "$f")"
    for cc in "${compilers[@]}"; do
        if [ -n "$only" ] && [ "$cc" != "$only" ]; then
            echo "skip  $name [$cc]: case applies only to $only"
            continue
        fi
        key=GCC; [ "$cc" = "clang++" ] && key=CLANG
        expect="$(header "EXPECT-$key" "$f")"
        mapfile -t flags < <(strict_flags "$cc" "$std")
        read -ra more <<<"$(header FLAGS "$f")"
        flags+=("${more[@]}")
        mapfile -t extra < <(link_files "$f")
        if [ "${#extra[@]}" -gt 0 ]; then
            mode=(-o "$tmp/link" "${extra[@]}")   # kompajliraj i linkuj
        else
            # -c, ne -fsyntax-only: neka upozorenja (npr. -Wreturn-type u
            # g++) nastaju tek u kasnijoj fazi kompajliranja.
            mode=(-c -o "$tmp/obj.o")
        fi
        if out="$("$cc" "${flags[@]}" "${mode[@]}" "$f" 2>&1)"; then
            echo "FAIL  $name [$cc]: compiled, but must not"; fail=1
        elif ! grep -qF -- "$expect" <<<"$out"; then
            echo "FAIL  $name [$cc]: fails, but not for the expected reason (\"$expect\"):"
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
    only="$(header ONLY-CC "$f")"
    san=(-fsanitize=address,undefined); san_name="ASan/UBSan"
    [ "$(header SANITIZER "$f")" = thread ] && { san=(-fsanitize=thread); san_name="TSan"; }
    for cc in "${compilers[@]}"; do
        if [ -n "$only" ] && [ "$cc" != "$only" ]; then
            echo "skip  $name [$cc]: case applies only to $only"
            continue
        fi
        # Sanitizer runtime nije uvek instaliran za svaki kompajler -- proveri.
        if ! echo 'int main(){}' | "$cc" -x c++ "${san[@]}" - -o "$tmp/probe" >/dev/null 2>&1; then
            echo "skip  $name [$cc]: no $san_name runtime for this compiler"
            continue
        fi
        mapfile -t flags < <(strict_flags "$cc" "$std")
        read -ra more <<<"$(header FLAGS "$f")"
        flags+=("${more[@]}")
        mapfile -t extra < <(link_files "$f")
        mapfile -t libs < <(tbb_libs "$cc" "$f")
        if ! "$cc" "${flags[@]}" -g -O0 "${san[@]}" -fno-omit-frame-pointer \
                "$f" "${extra[@]}" -o "$tmp/ub" "${libs[@]}" 2>"$tmp/cc.log"; then
            echo "FAIL  $name [$cc]: does not compile, but should"
            grep -m2 'error' "$tmp/cc.log"; fail=1
            continue
        fi
        out="$("$tmp/ub" 2>&1)"
        if grep -qE -- "$expect" <<<"$out"; then
            echo "ok    $name [$cc]"
        else
            echo "FAIL  $name [$cc]: sanitizer did not report \"$expect\""; fail=1
        fi
    done
done

for f in "$lesson"/runtime/*.cpp; do
    [ -e "$f" ] || continue
    name="runtime/$(basename "$f")"
    std="$(header STD "$f")"; std="${std:-c++17}"
    expect="$(header EXPECT-RUN "$f")"
    only="$(header ONLY-CC "$f")"
    for cc in "${compilers[@]}"; do
        if [ -n "$only" ] && [ "$cc" != "$only" ]; then
            echo "skip  $name [$cc]: case applies only to $only"
            continue
        fi
        mapfile -t flags < <(strict_flags "$cc" "$std")
        read -ra more <<<"$(header FLAGS "$f")"
        flags+=("${more[@]}")
        mapfile -t libs < <(tbb_libs "$cc" "$f")
        if ! "$cc" "${flags[@]}" -g -O0 "$f" -o "$tmp/run" "${libs[@]}" 2>"$tmp/cc.log"; then
            echo "FAIL  $name [$cc]: does not compile, but should"
            grep -m2 'error' "$tmp/cc.log"; fail=1
            continue
        fi
        if out="$("$tmp/run" 2>&1)"; then
            echo "FAIL  $name [$cc]: program exited successfully, but should have aborted"; fail=1
        elif grep -qE -- "$expect" <<<"$out"; then
            echo "ok    $name [$cc]"
        else
            echo "FAIL  $name [$cc]: output does not contain \"$expect\""; fail=1
        fi
    done
done

exit "$fail"
