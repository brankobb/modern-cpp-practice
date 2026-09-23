// VRSTA: zašto
// DEMO-OUT: NAIVNO nivo = 44
// DEMO-ERR: ZAGRADE narrowing|cannot be narrowed
//
// Zadatak 2 -- zašto {} zabranjuje narrowing (sekcija 6)
// Rešenje: exercises/solutions/z2_narrowing_senzor.cpp
//
// Senzor (10-bitni ADC) vraća 0..1023, a displej prima nivo 0..255.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week0-fundamentals/03-uniform-init/exercises/z2_narrowing_senzor.cpp -DNAIVNO
//   Ispiše "nivo = 44". Odakle 44? Da li je kompajler išta rekao?
//   (odgovor: 300 mod 256 = 44; bez upozorenja, ni sa -Wall -Wextra)
// Korak 2: ista linija sa {}:
//     ./build.sh .../z2_narrowing_senzor.cpp -DZAGRADE
//   Sada je greška. {} ne zna šta si hteo (skaliranje? odsecanje? modulo?),
//   pa te tera da odlučiš sam.
// Korak 3: napiši unsigned char uNivo(int sirovo): vrednosti ispod 0
//   postaju 0, iznad 1023 postaju 1023, pa skaliraj na 0..255 (deli sa 4).
//   Konverziju na kraju piši eksplicitno (static_cast), jer si sada
//   proverio opseg. Otkomentariši test u main().

#include <iostream>

void prikazi(unsigned char nivo) { std::cout << "nivo = " << int(nivo) << '\n'; }

// TODO korak 3: unsigned char uNivo(int sirovo)

int main() {
    int sirovo = 300;
#if defined(NAIVNO)
    unsigned char nivo = sirovo;       // tiho odsecanje
    prikazi(nivo);
#elif defined(ZAGRADE)
    unsigned char nivo{sirovo};        // narrowing: greška
    prikazi(nivo);
#else
    (void)sirovo;
    // Korak 3 -- otkomentariši:
    // for (int s : {300, 1023, 2000, -5})
    //     prikazi(uNivo(s));
#endif
}

/* OČEKIVANI IZLAZ
nivo = 75
nivo = 255
nivo = 255
nivo = 0
*/
