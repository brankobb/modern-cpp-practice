// Rešenje zadatka ex2_delete_conversions.

#include <iostream>

// Ako je samo "void setSpeed(unsigned)" (nije dobro): int, double,
// char, bool... se tiho konvertuju, i -100 postane 4294967196.
// Treba ovako: tačan tip ide u non-template overload, a obrisani template
// uhvati sve ostale tipove i pretvori ih u grešku pri kompajliranju.
void setSpeed(unsigned rpm) { std::cout << "speed = " << rpm << " rpm\n"; }
template <typename T>
void setSpeed(T) = delete;

int main() {
#ifdef BAD_CALL
    setSpeed(-100);   // g++: "use of deleted function"; clang: "call to deleted function"
    setSpeed(2.9);
#endif
    setSpeed(1500u);
    // Korak 3: konverzija je sada vidljiva i stoji iza provere.
    int fromConfig = 1200;
    if (fromConfig >= 0) setSpeed(static_cast<unsigned>(fromConfig));
}
