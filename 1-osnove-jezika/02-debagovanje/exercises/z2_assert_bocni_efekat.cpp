// VRSTA: zašto
// DEMO-OUT: NDEBUG senzor inicijalizovan: ne
//
// Zadatak 2 -- zašto u assert-u nikad nema bočnog efekta (sekcija 7)
// Rešenje: exercises/solutions/z2_assert_bocni_efekat.cpp
//
// Korak 1: pokreni normalno, pa kao "release" build:
//     ./build.sh 1-osnove-jezika/02-debagovanje/exercises/z2_assert_bocni_efekat.cpp
//     ./build.sh 1-osnove-jezika/02-debagovanje/exercises/z2_assert_bocni_efekat.cpp -DNDEBUG
//   U debug build-u senzor je inicijalizovan. U release-u (-DNDEBUG) NIJE:
//   assert(x) se tada pretvori u ((void)0), pa se ceo izraz u zagradi --
//   zajedno sa pozivom inicijalizuj() -- ne izvrši. Program radi drugačije
//   baš u verziji koja ide korisnicima, a testiran je u debug verziji.
// Korak 2: popravi main tako da se inicijalizuj() UVEK pozove, a assert
//   samo proverava rezultat. Pazi: sa -DNDEBUG promenljiva koju proverava
//   samo assert postaje neiskorišćena (-Wunused-variable) -- [[maybe_unused]].
// Korak 3: (za razmišljanje) ako inicijalizacija može da ne uspe i u
//   release-u (npr. senzor nije priključen), da li je assert pravi alat?

#include <cassert>
#include <iostream>

bool inicijalizovan = false;

bool inicijalizuj() {
    inicijalizovan = true;   // npr. upis u registre senzora
    return true;
}

int main() {
    assert(inicijalizuj());
    std::cout << "senzor inicijalizovan: " << (inicijalizovan ? "da" : "ne") << '\n';
}

/* OČEKIVANI IZLAZ
senzor inicijalizovan: da
*/
