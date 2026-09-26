// STD: c++17
// EXPECT-GCC: must have 'int' as its argument
// EXPECT-CLANG: parameter of overloaded post-increment operator must have type 'int' (not 'double')
// POGREŠNO: postfiks ++ sa parametrom tipa double.
// Zašto: prefiks i postfiks ++ imaju isti simbol, pa ih jezik razlikuje po
//   lažnom parametru: operator++() je prefiks, operator++(int) postfiks.
//   int se ne koristi; nijedan drugi tip nije dozvoljen ([over.inc]).
// Ispravno: Counter operator++(int).
struct Counter {
    int n = 0;
    Counter operator++(double) {
        Counter old = *this;
        ++n;
        return old;
    }
};

int main() {}
