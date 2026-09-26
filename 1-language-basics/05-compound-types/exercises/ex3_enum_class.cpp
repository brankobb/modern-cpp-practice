// KIND: why
// DEMO-OUT: NAIVNO šaljem komandu 2 \(Resetuj\)
// DEMO-ERR: ENUM_CLASS cannot convert|no matching function
//
// Zadatak 3 -- zašto enum class (sekcija 4, EMC Item 10)
// Rešenje: exercises/solutions/ex3_enum_class.cpp
//
// Uređaj prijavljuje stanje (Stanje), a kontroleru šalje komande (Komanda).
// Stari C API prima komandu kao int.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/05-compound-types/exercises/ex3_enum_class.cpp -DNAIVNO
//   Programer je hteo da prijavi stanje Greska, a greškom je pozvao
//   posalji(). Kompajler ćuti, a uređaj dobije komandu Resetuj (obe
//   vrednosti su 2). Obični enum se tiho konvertuje u int.
// Korak 2: ista greška sa enum class:
//     ./build.sh .../ex3_enum_class.cpp -DENUM_CLASS
//   Sada je greška pri kompajliranju: enum class se NE konvertuje sam, pa
//   Stanje ne može da uđe tamo gde se očekuje Komanda.
// Korak 3: u #else grani napiši enum class Stanje i enum class Komanda
//   (oba sa podloženim tipom std::uint8_t), const char* ime(Komanda) sa
//   switch-om, i void posalji(Komanda k) koja ispiše i bajt koji ide na
//   žicu -- tu broj tražiš EKSPLICITNO (static_cast). Otkomentariši test.

#include <cstdint>
#include <iostream>

#if defined(NAIVNO)
enum Stanje { Iskljuceno, Rad, Greska };      // 0 1 2
enum Komanda { Stani, Kreni, Resetuj };       // 0 1 2
const char* const imenaKomandi[] = {"Stani", "Kreni", "Resetuj"};

void posalji(int komanda) {
    std::cout << "šaljem komandu " << komanda << " (" << imenaKomandi[komanda] << ")\n";
}

int main() {
    Stanje s = Greska;
    posalji(s);        // hteo sam prijaviStanje(s)...
}
#elif defined(ENUM_CLASS)
enum class Stanje { Iskljuceno, Rad, Greska };
enum class Komanda { Stani, Kreni, Resetuj };

void posalji(Komanda) {}

int main() {
    Stanje s = Stanje::Greska;
    posalji(s);        // greška: Stanje nije Komanda
}
#else
// TODO korak 3

int main() {
    // Korak 3 -- otkomentariši:
    // posalji(Komanda::Resetuj);
    // posalji(Komanda::Kreni);
    // std::cout << "sizeof(Komanda) = " << sizeof(Komanda) << '\n';
}
#endif

/* EXPECTED OUTPUT
šaljem komandu Resetuj, bajt 2
šaljem komandu Kreni, bajt 1
sizeof(Komanda) = 1
*/
