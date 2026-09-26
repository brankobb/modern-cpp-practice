// VRSTA: zašto
// DEMO-OUT: NAIVNO posle greške: kanali 4 5 6, ime staro
//
// Zadatak 3 -- zašto "sve ili ništa" (strong guarantee, sekcija 4, EC++ Item 29)
// Rešenje: exercises/solutions/z3_jaka_garancija.cpp
//
// primeni() menja konfiguraciju: nove kanale i novo ime. Ime se proverava
// i može da bude neispravno (baci izuzetak).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-zivotni-vek-i-resursi/21-raii/exercises/z3_jaka_garancija.cpp -DNAIVNO
//   Posle neuspelog primeni() konfiguracija ima NOVE kanale i STARO ime --
//   stanje koje nikad nije trebalo da postoji. Nema curenja (basic
//   guarantee je ispunjen), ali pozivalac ne zna u kakvom je stanju objekat.
// Korak 2: u #else grani napiši primeni() sa jakom garancijom:
//   napravi KOPIJU, promeni kopiju (tu sme da baci -- original je
//   netaknut), pa na kraju zameni original sa kopijom operacijom koja ne
//   baca (swap članova, ili std::swap na celom objektu -- move vector-a i
//   string-a je noexcept).

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct Konfiguracija {
    std::vector<int> kanali;
    std::string ime;
};

void proveriIme(const std::string& ime) {
    if (ime.empty()) throw std::invalid_argument("prazno ime");
}

void ispisi(const char* opis, const Konfiguracija& k) {
    std::cout << opis << ": kanali";
    for (int c : k.kanali) std::cout << ' ' << c;
    std::cout << ", ime " << k.ime << '\n';
}

#ifdef NAIVNO
void primeni(Konfiguracija& k, const std::vector<int>& kanali, const std::string& ime) {
    k.kanali = kanali;      // uspe
    proveriIme(ime);        // baci -- kanali su već promenjeni
    k.ime = ime;
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija ne radi ništa)
void primeni(Konfiguracija&, const std::vector<int>&, const std::string&) {}
#endif

int main() {
    Konfiguracija k{{1, 2, 3}, "staro"};
    try {
        primeni(k, {4, 5, 6}, "");
    } catch (const std::invalid_argument&) {
        ispisi("posle greške", k);
    }
    primeni(k, {7, 8}, "novo");
    ispisi("posle uspeha", k);
}

/* OČEKIVANI IZLAZ
posle greške: kanali 1 2 3, ime staro
posle uspeha: kanali 7 8, ime novo
*/
