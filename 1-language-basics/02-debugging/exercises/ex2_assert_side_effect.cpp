// KIND: why
// DEMO-OUT: NDEBUG sensor initialized: no
//
// Zadatak 2 -- zašto u assert-u nikad nema bočnog efekta (sekcija 7)
// Rešenje: exercises/solutions/ex2_assert_side_effect.cpp
//
// Korak 1: pokreni normalno, pa kao "release" build:
//     ./build.sh 1-language-basics/02-debugging/exercises/ex2_assert_side_effect.cpp
//     ./build.sh 1-language-basics/02-debugging/exercises/ex2_assert_side_effect.cpp -DNDEBUG
//   U debug build-u senzor je inicijalizovan. U release-u (-DNDEBUG) NIJE:
//   assert(x) se tada pretvori u ((void)0), pa se ceo izraz u zagradi --
//   zajedno sa pozivom initialize() -- ne izvrši. Program radi drugačije
//   baš u verziji koja ide korisnicima, a testiran je u debug verziji.
// Korak 2: popravi main tako da se initialize() UVEK pozove, a assert
//   samo proverava rezultat. Pazi: sa -DNDEBUG promenljiva koju proverava
//   samo assert postaje neiskorišćena (-Wunused-variable) -- [[maybe_unused]].
// Korak 3: (za razmišljanje) ako inicijalizacija može da ne uspe i u
//   release-u (npr. senzor nije priključen), da li je assert pravi alat?

#include <cassert>
#include <iostream>

bool initialized = false;

bool initialize() {
    initialized = true;   // npr. upis u registre senzora
    return true;
}

int main() {
    assert(initialize());
    std::cout << "sensor initialized: " << (initialized ? "yes" : "no") << '\n';
}

/* EXPECTED OUTPUT
sensor initialized: yes
*/
