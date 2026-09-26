// KIND: usage
//
// Zadatak 1 -- namespace, ADL, anonimni i inline namespace (sekcije 1, 3, 4, 6)
//   ./build.sh 1-language-basics/08-namespaces-and-linkage/exercises/ex1_namespace_adl.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_namespace_adl.cpp
//
// Korak 1: u ugnežđenom namespace-u company::drivers (C++17 zapis
//   "namespace a::b { }") napiši struct Uart { int id; bool ready; };
//   i void initialize(Uart& u) koja postavi ready = true.
//   Van namespace-a napravi alias: namespace drv = company::drivers;
// Korak 2: u ISTOM namespace-u napiši
//   std::ostream& operator<<(std::ostream&, const Uart&) koji ispiše
//   "uart#<id> ready" ili "uart#<id> off". U main() su pozivi
//   initialize(u) i std::cout << u NEKVALIFIKOVANI -- nađe ih ADL.
// Korak 3: brojač poziva initialize() drži u ANONIMNOM namespace-u
//   (internal linkage: nije vidljiv iz drugih .cpp fajlova), a
//   inline constexpr int maxUart = 4; u company::drivers. Na kraju dodaj
//   inline namespace v2 u company sa const char* version() { return "v2"; },
//   i običan namespace v1 sa version() koja vraća "v1".
//   company::version() tada bira v2 bez pisanja v2.

#include <iostream>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 i 2 -- otkomentariši:
    // drv::Uart a{1, false}, b{2, false}, c{3, false};
    // initialize(a);            // ADL: a je tipa company::drivers::Uart
    // initialize(b);
    // std::cout << a << '\n' << b << '\n' << c << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << "initialized: " << initializationCount() << " of "
    //           << drv::maxUart << '\n';
    // std::cout << "version: " << company::version() << ", old: "
    //           << company::v1::version() << '\n';
}

/* EXPECTED OUTPUT
uart#1 ready
uart#2 ready
uart#3 off
initialized: 2 of 4
version: v2, old: v1
*/
