// KIND: why
// DEMO-ERR: NAIVE read-only|non-mutable lambda
// DEMO-OUT: MUTABLE gen\(\) after: 1
//
// Zadatak 3 -- zašto je operator() lambde podrazumevano const (sekcija 11)
// Rešenje: exercises/solutions/ex3_mutable_lambda.cpp
//
// Hoćemo generator ID-jeva: svaki poziv vrati sledeći broj.
//
// Korak 1: -DNAIVE -- lambda sa [id] pokuša da uveća svoju kopiju:
//     ./build.sh 1-language-basics/09-const/exercises/ex3_mutable_lambda.cpp -DNAIVE
//   Greška: operator() lambde je const, pa su zarobljene kopije read-only.
// Korak 2: -DMUTABLE -- sa mutable se kompajlira, ali:
//     ./build.sh .../ex3_mutable_lambda.cpp -DMUTABLE
//   generator se prosledi po vrednosti (kao što to rade i STL algoritmi),
//   pozove se 3 puta, a posle toga gen() ipak vrati 1. Stanje je živelo u
//   KOPIJI lambde. Upravo zato je podrazumevano const: lambda je objekat
//   koji se slobodno kopira, pa skriveno promenljivo stanje iznenađuje.
// Korak 3: u #else grani napiši dve ispravne verzije:
//   a) stanje van lambde: int next = 0; auto gen = [&next] { ... };
//      (lambda bez mutable; menja se promenljiva na koju referenca pokazuje)
//   b) mutable lambda, ali je prosledi po referenci: napiši
//      template <typename F> void callThreeTimesRef(F& f)
//   Otkomentariši test.

#include <iostream>

template <typename F>
void callThreeTimes(F f) {                  // po vrednosti: radi na kopiji
    for (int i = 0; i < 3; ++i) f();
}

int main() {
#if defined(NAIVE)
    int id = 0;
    auto gen = [id]() { return ++id; };     // greška: id je const u lambdi
    std::cout << gen() << '\n';
#elif defined(MUTABLE)
    auto gen = [id = 0]() mutable { return ++id; };
    callThreeTimes(gen);
    std::cout << "gen() after: " << gen() << '\n';
#else
    // TODO korak 3
    // Korak 3 -- otkomentariši:
    // int next = 0;
    // auto genA = [&next] { return ++next; };
    // callThreeTimes(genA);                 // kopija lambde, ali ista referenca
    // std::cout << "a) genA() after: " << genA() << '\n';
    //
    // auto genB = [id = 0]() mutable { return ++id; };
    // callThreeTimesRef(genB);
    // std::cout << "b) genB() after: " << genB() << '\n';
#endif
}

/* EXPECTED OUTPUT
a) genA() after: 4
b) genB() after: 4
*/
