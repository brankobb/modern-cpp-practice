// VRSTA: upotreba
//
// Zadatak 1 -- redosled pravljenja i uništavanja (sekcije 2, 3)
//   ./build.sh 3-zivotni-vek-i-resursi/19-zivotni-vek-objekta/exercises/z1_redosled_traga.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_redosled_traga.cpp
//
// Korak 1: class Trag -- explicit Trag(const char* ime) ispiše " ime()",
//   destruktor ispiše " ~ime()" (razmak ISPRED, bez novog reda).
//   Kopiranje zabrani.
// Korak 2: class Motor nasleđuje Trag (bazi prosledi ime "baza"), ima
//   članove Trag filter_ i Trag pumpa_ (TIM redom deklarisane), a u init
//   listi ih napiši obrnuto: pumpa_("pumpa"), filter_("filter"). Telo
//   konstruktora ispiše " telo", destruktor " ~telo". Kompajler upozori
//   (-Wreorder) -- pročitaj, pa ispravi init listu da prati deklaraciju.
// Korak 3: PRE pokretanja, za svaki blok testa napiši u komentar
//   predviđanje izlaza. Onda otkomentariši i uporedi.

#include <iostream>

// TODO korak 1 i 2

int main() {
    // Korak 3 -- predvidi, pa otkomentariši:
    // std::cout << "blok:";
    // {
    //     Trag a("a");
    //     Trag b("b");
    // }
    // std::cout << "\nniz:";
    // {
    //     Trag niz[] = {Trag("x0"), Trag("x1"), Trag("x2")};   // C++17: bez kopija
    // }
    // std::cout << "\nklasa:";
    // {
    //     Motor m;
    // }
    // std::cout << "\nprivremeni:";
    // {
    //     Trag("tmp"), std::cout << " isti-izraz";   // privremeni živi do kraja izraza (;)
    //     std::cout << " sledeći-izraz";
    // }
    // std::cout << '\n';
}

/* OČEKIVANI IZLAZ
blok: a() b() ~b() ~a()
niz: x0() x1() x2() ~x2() ~x1() ~x0()
klasa: baza() filter() pumpa() telo ~telo ~pumpa() ~filter() ~baza()
privremeni: tmp() isti-izraz ~tmp() sledeći-izraz
*/
