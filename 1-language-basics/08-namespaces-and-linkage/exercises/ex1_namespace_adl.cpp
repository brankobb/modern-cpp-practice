// KIND: usage
//
// Zadatak 1 -- namespace, ADL, anonimni i inline namespace (sekcije 1, 3, 4, 6)
//   ./build.sh 1-language-basics/08-namespaces-and-linkage/exercises/ex1_namespace_adl.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_namespace_adl.cpp
//
// Korak 1: u ugnežđenom namespace-u firma::drajveri (C++17 zapis
//   "namespace a::b { }") napiši struct Uart { int id; bool spreman; };
//   i void inicijalizuj(Uart& u) koja postavi spreman = true.
//   Van namespace-a napravi alias: namespace drv = firma::drajveri;
// Korak 2: u ISTOM namespace-u napiši
//   std::ostream& operator<<(std::ostream&, const Uart&) koji ispiše
//   "uart#<id> spreman" ili "uart#<id> ugašen". U main() su pozivi
//   inicijalizuj(u) i std::cout << u NEKVALIFIKOVANI -- nađe ih ADL.
// Korak 3: brojač poziva inicijalizuj() drži u ANONIMNOM namespace-u
//   (internal linkage: nije vidljiv iz drugih .cpp fajlova), a
//   inline constexpr int maxUart = 4; u firma::drajveri. Na kraju dodaj
//   inline namespace v2 u firma sa const char* verzija() { return "v2"; },
//   i običan namespace v1 sa verzija() koja vraća "v1".
//   firma::verzija() tada bira v2 bez pisanja v2.

#include <iostream>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 i 2 -- otkomentariši:
    // drv::Uart a{1, false}, b{2, false}, c{3, false};
    // inicijalizuj(a);          // ADL: a je tipa firma::drajveri::Uart
    // inicijalizuj(b);
    // std::cout << a << '\n' << b << '\n' << c << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << "inicijalizovano: " << brojInicijalizacija() << " od "
    //           << drv::maxUart << '\n';
    // std::cout << "verzija: " << firma::verzija() << ", stara: "
    //           << firma::v1::verzija() << '\n';
}

/* EXPECTED OUTPUT
uart#1 spreman
uart#2 spreman
uart#3 ugašen
inicijalizovano: 2 od 4
verzija: v2, stara: v1
*/
