// Rešenje zadatka ex2_delete_konverzije.

#include <iostream>

// Ako je samo "void postaviBrzinu(unsigned)" (nije dobro): int, double,
// char, bool... se tiho konvertuju, i -100 postane 4294967196.
// Treba ovako: tačan tip ide u non-template overload, a obrisani template
// uhvati sve ostale tipove i pretvori ih u grešku pri kompajliranju.
void postaviBrzinu(unsigned rpm) { std::cout << "brzina = " << rpm << " rpm\n"; }
template <typename T>
void postaviBrzinu(T) = delete;

int main() {
#ifdef LOS_POZIV
    postaviBrzinu(-100);   // g++: "use of deleted function"; clang: "call to deleted function"
    postaviBrzinu(2.9);
#endif
    postaviBrzinu(1500u);
    // Korak 3: konverzija je sada vidljiva i stoji iza provere.
    int izKonfiguracije = 1200;
    if (izKonfiguracije >= 0) postaviBrzinu(static_cast<unsigned>(izKonfiguracije));
}
