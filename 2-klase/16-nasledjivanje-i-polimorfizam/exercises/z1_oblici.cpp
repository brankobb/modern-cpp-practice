// VRSTA: upotreba
//
// Zadatak 1 -- interfejs, override, final, virtualni destruktor i clone()
// (sekcije 4, 5, 9, 12)
//   ./build.sh 2-klase/16-nasledjivanje-i-polimorfizam/exercises/z1_oblici.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_oblici.cpp
//
// Korak 1: apstraktna klasa Oblik: čiste virtualne funkcije
//   double povrsina() const i std::string ime() const, i virtualni
//   destruktor (= default). Kopiranje zabrani (C.67: polimorfna klasa se
//   ne kopira direktno -- zato postoji clone() u koraku 3).
// Korak 2: Krug(double r) i Pravougaonik(double a, double b) nasleđuju
//   Oblik (public). Kvadrat(double a) nasleđuje Pravougaonik i on je final.
//   Svaka funkcija koja nadjačava ima override. Kvadrat menja samo ime().
//   Za pi koristi 3.14159.
// Korak 3: virtual std::unique_ptr<Oblik> clone() const = 0; u Obliku, i
//   implementacija u svakoj klasi (return std::make_unique<Krug>(*this);
//   -- izvedena klasa SME da se kopira, pa joj treba protected copy
//   konstruktor u Obliku umesto = delete).

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 i 2 -- otkomentariši:
    // std::vector<std::unique_ptr<Oblik>> oblici;
    // oblici.push_back(std::make_unique<Krug>(1.0));
    // oblici.push_back(std::make_unique<Pravougaonik>(2.0, 3.0));
    // oblici.push_back(std::make_unique<Kvadrat>(4.0));
    // double ukupno = 0;
    // for (const auto& o : oblici) {
    //     std::cout << o->ime() << ": " << o->povrsina() << '\n';
    //     ukupno += o->povrsina();
    // }
    // std::cout << "ukupno: " << ukupno << '\n';

    // Korak 3 -- otkomentariši:
    // std::unique_ptr<Oblik> kopija = oblici[2]->clone();
    // std::cout << "kopija: " << kopija->ime() << ' ' << kopija->povrsina() << '\n';
}

/* OČEKIVANI IZLAZ
krug: 3.14159
pravougaonik: 6
kvadrat: 16
ukupno: 25.1416
kopija: kvadrat 16
*/
