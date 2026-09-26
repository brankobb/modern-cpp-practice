#!/usr/bin/env bash
# Proverava VEŽBE lekcija, sa g++ i clang++ (šta je instalirano), C++17 i C++20.
#   <lekcija>/exercises/zN_ime.cpp            -- zadatak: mora da se kompajlira
#                                                i radi i ovakav, nerešen
#   <lekcija>/exercises/solutions/zN_ime.cpp  -- rešenje: izlaz mora da bude
#                                                tačno blok "OČEKIVANI IZLAZ"
#                                                iz zadatka
# Oba fajla: strogi flegovi + -Werror, pod ASan/UBSan (gde ima runtime-a).
#
# Zaglavlja u fajlu zadatka:
#   // VRSTA: upotreba | zašto   -- lekcija mora da ima bar jedan od oba
#   // STD: c++20                -- samo taj standard (podrazumevano oba)
#   // FLAGS: ...                -- dodatni flegovi za sve build-ove
#   // SANITIZER: thread         -- ThreadSanitizer umesto ASan/UBSan za sve
#                                   build-ove tog zadatka (i za DEMO-UB)
#   // DEMO-ERR: MAKRO regex     -- sa -DMAKRO zadatak NE SME da se kompajlira,
#                                   a poruka mora da odgovara regex-u (oba
#                                   kompajlera, pa regex pokriva obe poruke)
#   // DEMO-UB: MAKRO regex      -- sa -DMAKRO se kompajlira, a ASan/UBSan
#                                   pri pokretanju prijavi regex
#   // DEMO-OUT: MAKRO regex     -- sa -DMAKRO se kompajlira i radi, a izlaz
#                                   odgovara regex-u (tihi pogrešan rezultat)
#   DEMO-UB i DEMO-OUT se grade BEZ -Werror: loš kod sme da izazove
#   upozorenje (i to je deo lekcije), ali mora da se kompajlira.
# Blok očekivanog izlaza, na kraju fajla zadatka:
#   /* OČEKIVANI IZLAZ
#   ...
#   */
# Završna vežba dela (<deo>/zavrsna-vezba/): zadatak.cpp (kostur, sa blokom
# OČEKIVANI IZLAZ i istim zaglavljima) i resenje.cpp; pravilo o vrstama
# zadataka za nju ne važi.
# Usage: ./check_exercises.sh [<folder lekcije ili zavrsna-vezba>...]   (bez argumenata: sve)
set -u
cd "$(dirname "$0")"

lessons=("$@")
if [ ${#lessons[@]} -eq 0 ]; then
    mapfile -t lessons < <({ find . -path '*/exercises' -type d -printf '%h\n'
                             find . -type d -name zavrsna-vezba; } | sed 's|^\./||' | sort)
fi

fail=0
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT

compilers=()
for cc in g++ clang++; do
    command -v "$cc" >/dev/null 2>&1 && compilers+=("$cc")
done

declare -A has_san   # ključ: "kompajler asan" ili "kompajler thread"
for cc in "${compilers[@]}"; do
    if echo 'int main(){}' | "$cc" -x c++ -fsanitize=address,undefined - -o "$tmp/probe" >/dev/null 2>&1; then
        has_san[$cc asan]=1
    else
        has_san[$cc asan]=0
        echo "napomena: $cc nema ASan/UBSan runtime -- gradi se bez sanitizera, DEMO-UB se preskače"
    fi
    if echo 'int main(){}' | "$cc" -x c++ -fsanitize=thread - -o "$tmp/probe" >/dev/null 2>&1; then
        has_san[$cc thread]=1
    else
        has_san[$cc thread]=0
    fi
done

header() { sed -n "s|^// $1: *||p" "$2"; }

tbb_libs() { # cc fajl -> -ltbb ako fajl koristi <execution>, a libstdc++ izabere TBB
    grep -q '#include <execution>' "$2" &&
        echo '#include <execution>' | "$1" -std=c++17 -x c++ -dM -E - 2>/dev/null | grep -q _PSTL_PAR_BACKEND_TBB &&
        echo -ltbb
    return 0
}

flags_for() { # cc std san -> strogi flegovi kao build.sh, plus -Werror
    local flags=(-std="$2" -Wall -Wextra -Wshadow -pedantic-errors -Werror=vla -Werror -g -O0)
    [ "$1" = "clang++" ] && flags+=(-Werror=reorder-init-list)
    if [ "${has_san[$1 $3]}" = 1 ]; then
        if [ "$3" = thread ]; then
            flags+=(-fsanitize=thread -fno-omit-frame-pointer)
        else
            flags+=(-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer)
        fi
    fi
    printf '%s\n' "${flags[@]}"
}

expected_output() { sed -n '/^\/\* OČEKIVANI IZLAZ/,/^\*\//p' "$1" | sed '1d;$d'; }

for lesson in "${lessons[@]}"; do
    lesson="${lesson%/}"
    echo "== $lesson"
    kinds=""
    pairs=()
    zavrsna=0
    if [ "$(basename "$lesson")" = zavrsna-vezba ]; then
        zavrsna=1
        pairs=("$lesson/zadatak.cpp|$lesson/resenje.cpp")
    else
        for f in "$lesson"/exercises/*.cpp; do
            [ -e "$f" ] && pairs+=("$f|$lesson/exercises/solutions/$(basename "$f")")
        done
    fi
    for pair in ${pairs[@]+"${pairs[@]}"}; do
        f="${pair%%|*}"
        sol="${pair#*|}"
        if [ ! -e "$f" ]; then echo "FAIL  $f ne postoji"; fail=1; continue; fi
        name="$(basename "$f")"
        kinds+=" $(header VRSTA "$f")"
        if [ ! -e "$sol" ]; then echo "FAIL  $name: nema rešenja $sol"; fail=1; continue; fi
        expect="$(expected_output "$f")"
        if [ -z "$expect" ]; then echo "FAIL  $name: nema bloka OČEKIVANI IZLAZ"; fail=1; fi
        std_hdr="$(header STD "$f")"
        stds=(c++17 c++20); [ -n "$std_hdr" ] && stds=("$std_hdr")
        read -ra more <<<"$(header FLAGS "$f")"
        san=asan; [ "$(header SANITIZER "$f")" = thread ] && san=thread
        if [ "$san" = thread ]; then
            for cc in "${compilers[@]}"; do
                [ "${has_san[$cc thread]}" = 1 ] || echo "napomena: $cc nema TSan runtime -- $name se gradi bez sanitizera, DEMO-UB se preskače"
            done
        fi

        for cc in "${compilers[@]}"; do
            for std in "${stds[@]}"; do
                tag="[$cc $std]"
                mapfile -t flags < <(flags_for "$cc" "$std" "$san")
                flags+=("${more[@]}")
                mapfile -t libs < <(tbb_libs "$cc" "$f")
                ok=1
                # 1. zadatak, nerešen: kompajlira se bez upozorenja i radi čisto
                if ! "$cc" "${flags[@]}" "$f" -o "$tmp/z" "${libs[@]}" 2>"$tmp/cc.log"; then
                    echo "FAIL  $name $tag: zadatak se ne kompajlira"; head -5 "$tmp/cc.log"; ok=0
                elif ! "$tmp/z" >/dev/null 2>"$tmp/run.log"; then
                    echo "FAIL  $name $tag: zadatak pada pri pokretanju"; head -5 "$tmp/run.log"; ok=0
                fi
                # 2. rešenje: kompajlira se, radi čisto, izlaz = očekivani
                if ! "$cc" "${flags[@]}" "$sol" -o "$tmp/r" "${libs[@]}" 2>"$tmp/cc.log"; then
                    echo "FAIL  $name $tag: rešenje se ne kompajlira"; head -5 "$tmp/cc.log"; ok=0
                elif ! out="$("$tmp/r" 2>"$tmp/run.log")"; then
                    echo "FAIL  $name $tag: rešenje pada pri pokretanju"; head -5 "$tmp/run.log"; ok=0
                elif [ "$out" != "$expect" ]; then
                    echo "FAIL  $name $tag: izlaz rešenja se razlikuje od OČEKIVANI IZLAZ:"
                    diff <(echo "$expect") <(echo "$out") | head -10; ok=0
                fi
                # 3. demonstracije problema
                while read -r macro regex; do
                    [ -n "$macro" ] || continue
                    if out="$("$cc" "${flags[@]}" -D"$macro" -fsyntax-only "$f" 2>&1)"; then
                        echo "FAIL  $name $tag: -D$macro se kompajlira, a ne bi smelo"; ok=0
                    elif ! grep -qE -- "$regex" <<<"$out"; then
                        echo "FAIL  $name $tag: -D$macro pada, ali ne sa \"$regex\":"
                        grep -m2 'error' <<<"$out"; ok=0
                    fi
                done < <(header DEMO-ERR "$f")
                demo_flags=()
                for x in "${flags[@]}"; do [ "$x" = -Werror ] || demo_flags+=("$x"); done
                while read -r macro regex; do
                    [ -n "$macro" ] || continue
                    [ "${has_san[$cc $san]}" = 1 ] || continue
                    if ! "$cc" "${demo_flags[@]}" -D"$macro" "$f" -o "$tmp/d" "${libs[@]}" 2>"$tmp/cc.log"; then
                        echo "FAIL  $name $tag: -D$macro se ne kompajlira"; head -5 "$tmp/cc.log"; ok=0
                    elif ! grep -qE -- "$regex" <<<"$("$tmp/d" 2>&1)"; then
                        echo "FAIL  $name $tag: -D$macro: sanitizer nije prijavio \"$regex\""; ok=0
                    fi
                done < <(header DEMO-UB "$f")
                while read -r macro regex; do
                    [ -n "$macro" ] || continue
                    if ! "$cc" "${demo_flags[@]}" -D"$macro" "$f" -o "$tmp/d" "${libs[@]}" 2>"$tmp/cc.log"; then
                        echo "FAIL  $name $tag: -D$macro se ne kompajlira"; head -5 "$tmp/cc.log"; ok=0
                    elif ! out="$("$tmp/d" 2>&1)"; then
                        echo "FAIL  $name $tag: -D$macro pada pri pokretanju"; ok=0
                    elif ! grep -qE -- "$regex" <<<"$out"; then
                        echo "FAIL  $name $tag: -D$macro: izlaz ne sadrži \"$regex\""; ok=0
                    fi
                done < <(header DEMO-OUT "$f")
                if [ "$ok" = 1 ]; then echo "ok    $name $tag"; else fail=1; fi
            done
        done
    done
    if [ "$zavrsna" = 0 ] && [[ "$kinds" != *upotreba* || "$kinds" != *zašto* ]]; then
        echo "FAIL  $lesson: treba bar jedan zadatak VRSTA: upotreba i bar jedan VRSTA: zašto"
        fail=1
    fi
done

exit "$fail"
