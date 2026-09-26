// KIND: why
// DEMO-UB: NAIVE stack-use-after-scope
//
// Zadatak 3 -- const& produžava život privremenog, ali ne kroz funkciju
// (sekcija 8)
// Rešenje: exercises/solutions/ex3_lifetime_extension.cpp
//
// Korak 1: PREDVIDI pa pokreni. Obe linije izgledaju isto: const int&
//   vezan za privremenu vrednost.
//     ./build.sh 1-language-basics/04-pointers-and-references/exercises/ex3_lifetime_extension.cpp -DNAIVE
//   Prva radi, druga je UB (ASan: stack-use-after-scope). g++ uz to
//   upozori (-Wdangling-reference), clang ne. Zašto je razlika?
//   Pravilo ([class.temporary]): život se produžava samo kad se privremeni
//   veže DIREKTNO za referencu. larger() vraća referencu na svoj parametar,
//   a kompajler u trenutku poziva ne zna da je to privremeni objekat.
// Korak 2: u #else grani napiši ispravno: rezultat funkcije čuvaj po
//   vrednosti. Zatim napiši i int& largerRef(int& a, int& b), koja vraća
//   referencu na veću PROMENLJIVU i preko nje je uveća za 10 (ovde je
//   referenca na izlazu korisna, jer argumenti žive duže od poziva).

#include <iostream>

const int& larger(const int& a, const int& b) { return a > b ? a : b; }

int main() {
#ifdef NAIVE
    const int& direct = 1 + 2;              // OK: privremeni živi koliko i referenca
    const int& throughCall = larger(1, 2);  // privremeni 1 i 2 nestaju na kraju izraza
    std::cout << direct << ' ' << throughCall << '\n';
#else
    // TODO korak 2
    // Korak 2 -- otkomentariši:
    // int x = 3, y = 7;
    // int m = larger(x + 1, y + 1);       // po vrednosti: kopija pre kraja izraza
    // std::cout << "larger: " << m << '\n';
    // largerRef(x, y) += 10;
    // std::cout << "x=" << x << " y=" << y << '\n';
#endif
}

/* EXPECTED OUTPUT
larger: 8
x=3 y=17
*/
