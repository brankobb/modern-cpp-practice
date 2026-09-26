// KIND: why
// DEMO-UB: NAIVNO heap-use-after-free
//
// Zadatak 2 -- zašto string_view ne sme da bude član koji "čuva" ime (sekcija 3; ub/u01)
// Rešenje: exercises/solutions/ex2_pogled_u_prazno.cpp
//
// Kanal pamti svoje ime. Ime se sastavlja u funkciji napravi().
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 7-standard-library/38-string-view-and-filesystem/exercises/ex2_pogled_u_prazno.cpp -DNAIVNO
//   ASan: heap-use-after-free. Član ime je string_view -- pokazuje na
//   lokalni std::string iz napravi(), koji je uništen na izlasku iz
//   funkcije. Kanal "ima ime" koje više ne postoji.
//   Bez ASan-a (npr. g++ -std=c++17 -DNAIVNO ...) ispis je smeće:
//   nasumični bajtovi i komad imena drugog kanala. Šta se tačno vidi
//   zavisi od toga šta je u međuvremenu prepisalo tu memoriju.
// Korak 2: u #else grani napiši Kanal i napravi() tako da Kanal POSEDUJE
//   ime. Pravilo: string_view za parametre i kratkotrajne poglede, nikad
//   za podatak koji objekat treba da čuva.

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#ifdef NAIVNO
struct Kanal {
    std::string_view ime;
    int broj;
};
Kanal napravi(int broj) {
    std::string ime = "senzor_temperature_hala_" + std::to_string(broj);
    return Kanal{ime, broj};
}
#else
// TODO korak 2 (dok ne napišeš, sva imena su "?")
struct Kanal {
    std::string_view ime;
    int broj;
};
Kanal napravi(int broj) { return Kanal{"?", broj}; }
#endif

int main() {
    std::vector<Kanal> kanali;
    for (int i = 1; i <= 2; ++i) kanali.push_back(napravi(i));
    for (const auto& k : kanali) std::cout << k.broj << ": " << k.ime << '\n';
}

/* EXPECTED OUTPUT
1: senzor_temperature_hala_1
2: senzor_temperature_hala_2
*/
