// KIND: why
// DEMO-ERR: NAIVNO read-only|non-mutable lambda
// DEMO-OUT: MUTABLE gen\(\) posle: 1
//
// Zadatak 3 -- zašto je operator() lambde podrazumevano const (sekcija 11)
// Rešenje: exercises/solutions/ex3_mutable_lambda.cpp
//
// Hoćemo generator ID-jeva: svaki poziv vrati sledeći broj.
//
// Korak 1: -DNAIVNO -- lambda sa [id] pokuša da uveća svoju kopiju:
//     ./build.sh 1-language-basics/09-const/exercises/ex3_mutable_lambda.cpp -DNAIVNO
//   Greška: operator() lambde je const, pa su zarobljene kopije read-only.
// Korak 2: -DMUTABLE -- sa mutable se kompajlira, ali:
//     ./build.sh .../ex3_mutable_lambda.cpp -DMUTABLE
//   generator se prosledi po vrednosti (kao što to rade i STL algoritmi),
//   pozove se 3 puta, a posle toga gen() ipak vrati 1. Stanje je živelo u
//   KOPIJI lambde. Upravo zato je podrazumevano const: lambda je objekat
//   koji se slobodno kopira, pa skriveno promenljivo stanje iznenađuje.
// Korak 3: u #else grani napiši dve ispravne verzije:
//   a) stanje van lambde: int sledeci = 0; auto gen = [&sledeci] { ... };
//      (lambda bez mutable; menja se promenljiva na koju referenca pokazuje)
//   b) mutable lambda, ali je prosledi po referenci: napiši
//      template <typename F> void pozoviTriPutaRef(F& f)
//   Otkomentariši test.

#include <iostream>

template <typename F>
void pozoviTriPuta(F f) {                   // po vrednosti: radi na kopiji
    for (int i = 0; i < 3; ++i) f();
}

int main() {
#if defined(NAIVNO)
    int id = 0;
    auto gen = [id]() { return ++id; };     // greška: id je const u lambdi
    std::cout << gen() << '\n';
#elif defined(MUTABLE)
    auto gen = [id = 0]() mutable { return ++id; };
    pozoviTriPuta(gen);
    std::cout << "gen() posle: " << gen() << '\n';
#else
    // TODO korak 3
    // Korak 3 -- otkomentariši:
    // int sledeci = 0;
    // auto genA = [&sledeci] { return ++sledeci; };
    // pozoviTriPuta(genA);                  // kopija lambde, ali ista referenca
    // std::cout << "a) genA() posle: " << genA() << '\n';
    //
    // auto genB = [id = 0]() mutable { return ++id; };
    // pozoviTriPutaRef(genB);
    // std::cout << "b) genB() posle: " << genB() << '\n';
#endif
}

/* EXPECTED OUTPUT
a) genA() posle: 4
b) genB() posle: 4
*/
