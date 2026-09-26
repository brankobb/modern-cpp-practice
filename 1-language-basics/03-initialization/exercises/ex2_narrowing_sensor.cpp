// KIND: why
// DEMO-OUT: NAIVE level = 44
// DEMO-ERR: BRACES narrowing|cannot be narrowed
//
// Zadatak 2 -- zašto {} zabranjuje narrowing (sekcija 6)
// Rešenje: exercises/solutions/ex2_narrowing_sensor.cpp
//
// Senzor (10-bitni ADC) vraća 0..1023, a displej prima nivo 0..255.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/03-initialization/exercises/ex2_narrowing_sensor.cpp -DNAIVE
//   Ispiše "level = 44". Odakle 44? Da li je kompajler išta rekao?
//   (odgovor: 300 mod 256 = 44; bez upozorenja, ni sa -Wall -Wextra)
// Korak 2: ista linija sa {}:
//     ./build.sh .../ex2_narrowing_sensor.cpp -DBRACES
//   Sada je greška. {} ne zna šta si hteo (skaliranje? odsecanje? modulo?),
//   pa te tera da odlučiš sam.
// Korak 3: napiši unsigned char toLevel(int raw): vrednosti ispod 0
//   postaju 0, iznad 1023 postaju 1023, pa skaliraj na 0..255 (deli sa 4).
//   Konverziju na kraju piši eksplicitno (static_cast), jer si sada
//   proverio opseg. Otkomentariši test u main().

#include <iostream>

void show(unsigned char level) { std::cout << "level = " << int(level) << '\n'; }

// TODO korak 3: unsigned char toLevel(int raw)

int main() {
    int raw = 300;
#if defined(NAIVE)
    unsigned char level = raw;         // tiho odsecanje
    show(level);
#elif defined(BRACES)
    unsigned char level{raw};          // narrowing: greška
    show(level);
#else
    (void)raw;
    // Korak 3 -- otkomentariši:
    // for (int s : {300, 1023, 2000, -5})
    //     show(toLevel(s));
#endif
}

/* EXPECTED OUTPUT
level = 75
level = 255
level = 255
level = 0
*/
