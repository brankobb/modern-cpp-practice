// VRSTA: upotreba
//
// Zadatak 1 -- sopstvena klasa izuzetka, prevođenje izuzetaka i lanac
// uzroka (sekcije 2, 3, 6)
//   ./build.sh week3-advanced/s11-exceptions/exercises/z1_konfiguracija_greske.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_konfiguracija_greske.cpp
//
// Konfiguracija je niz redova oblika "kljuc=broj".
// Korak 1: class GreskaKonfiguracije nasleđuje std::runtime_error i pamti
//   broj reda: GreskaKonfiguracije(int red, const std::string& poruka),
//   what() vraća "red <red>: <poruka>", a int red() const noexcept.
// Korak 2: int procitajVrednost(const std::string& linija, int red) --
//   ako nema '=', baci GreskaKonfiguracije(red, "nema '='"). Broj posle
//   '=' pročitaj sa std::stoi. stoi baca std::invalid_argument ili
//   std::out_of_range -- uhvati ih (obe su std::logic_error) i PREVEDI u
//   GreskaKonfiguracije(red, "vrednost nije broj: '<tekst>'"). Pozivalac
//   ne treba da zna da unutra radi stoi.
// Korak 3: int ucitaj(const std::vector<std::string>& linije) sabira sve
//   vrednosti. Svaku grešku zamota sa
//   std::throw_with_nested(std::runtime_error("konfiguracija nije učitana")),
//   a u main-u ispiši lanac (std::rethrow_if_nested, kao u main.cpp
//   sekcija 6), uvučeno po 2 razmaka po nivou.

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// TODO korak 1, 2, 3

// void ispisiLanac(const std::exception& e, int nivo = 0) { ... }

int main() {
    // Korak 1-3 -- otkomentariši:
    // std::cout << "zbir: " << ucitaj({"a=10", "b=20", "c=30"}) << '\n';
    // for (const std::vector<std::string>& konfig :
    //      {std::vector<std::string>{"a=10", "b=abc"}, std::vector<std::string>{"a=10", "b 20"}}) {
    //     try {
    //         ucitaj(konfig);
    //     } catch (const std::exception& e) {
    //         ispisiLanac(e);
    //     }
    // }
}

/* OČEKIVANI IZLAZ
zbir: 60
konfiguracija nije učitana
  red 2: vrednost nije broj: 'abc'
konfiguracija nije učitana
  red 2: nema '='
*/
