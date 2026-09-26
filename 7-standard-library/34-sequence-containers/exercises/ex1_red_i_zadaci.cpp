// KIND: usage
//
// Zadatak 1 -- deque kao red, list sa splice, array kao brojač
// (sekcije 2, 4, 5)
//   ./build.sh 7-standard-library/34-sequence-containers/exercises/ex1_red_i_zadaci.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_red_i_zadaci.cpp
//
// Korak 1: class RedPoruka nad std::deque<std::string>:
//   void primi(std::string p) -- na kraj; void primiHitno(std::string p) --
//   na POČETAK; bool obradi(std::string& izlaz) -- skine prvu poruku
//   (false ako je red prazan). Zašto deque, a ne vector?
// Korak 2: lista zadataka std::list<std::string>. void naPocetak(
//   std::list<std::string>& l, const std::string& ime) -- nađi zadatak
//   (std::find) i PREMESTI ga na početak sa l.splice(l.begin(), l, it):
//   O(1), bez kopiranja stringa, a ostali iteratori ostaju važeći.
// Korak 3: std::array<int, 7> za broj poruka po danu u nedelji (0 =
//   ponedeljak). Nađi najprometniji dan: std::max_element i
//   std::distance za indeks.

#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <list>
#include <string>

// TODO korak 1 i 2

int main() {
    // Korak 1 -- otkomentariši:
    // RedPoruka red;
    // red.primi("temp 21");
    // red.primi("temp 22");
    // red.primiHitno("ALARM pritisak");
    // std::string p;
    // while (red.obradi(p)) std::cout << "obrađeno: " << p << '\n';

    // Korak 2 -- otkomentariši:
    // std::list<std::string> zadaci{"kalibracija", "log", "backup", "update"};
    // naPocetak(zadaci, "backup");
    // std::cout << "zadaci:";
    // for (const auto& z : zadaci) std::cout << ' ' << z;
    // std::cout << '\n';

    // Korak 3 -- otkomentariši:
    // std::array<int, 7> poDanu{12, 30, 7, 30, 45, 3, 0};
    // auto it = std::max_element(poDanu.begin(), poDanu.end());
    // std::cout << "najprometniji dan: " << std::distance(poDanu.begin(), it) << " (" << *it << " poruka)\n";
}

/* EXPECTED OUTPUT
obrađeno: ALARM pritisak
obrađeno: temp 21
obrađeno: temp 22
zadaci: backup kalibracija log update
najprometniji dan: 4 (45 poruka)
*/
