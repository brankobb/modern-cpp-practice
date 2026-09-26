// KIND: usage
//
// Zadatak 1 -- redosled pravljenja i uništavanja (sekcije 2, 3)
//   ./build.sh 3-lifetime-and-resources/19-object-lifetime/exercises/ex1_trace_order.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_trace_order.cpp
//
// Korak 1: class Trace -- explicit Trace(const char* name) ispiše " name()",
//   destruktor ispiše " ~name()" (razmak ISPRED, bez novog reda).
//   Kopiranje zabrani.
// Korak 2: class Engine nasleđuje Trace (bazi prosledi ime "base"), ima
//   članove Trace filter_ i Trace pump_ (TIM redom deklarisane), a u init
//   listi ih napiši obrnuto: pump_("pump"), filter_("filter"). Telo
//   konstruktora ispiše " body", destruktor " ~body". Kompajler upozori
//   (-Wreorder) -- pročitaj, pa ispravi init listu da prati deklaraciju.
// Korak 3: PRE pokretanja, za svaki blok testa napiši u komentar
//   predviđanje izlaza. Onda otkomentariši i uporedi.

#include <iostream>

// TODO korak 1 i 2

int main() {
    // Korak 3 -- predvidi, pa otkomentariši:
    // std::cout << "block:";
    // {
    //     Trace a("a");
    //     Trace b("b");
    // }
    // std::cout << "\narray:";
    // {
    //     Trace arr[] = {Trace("x0"), Trace("x1"), Trace("x2")};   // C++17: bez kopija
    // }
    // std::cout << "\nclass:";
    // {
    //     Engine e;
    // }
    // std::cout << "\ntemporary:";
    // {
    //     Trace("tmp"), std::cout << " same-expression";   // privremeni živi do kraja izraza (;)
    //     std::cout << " next-expression";
    // }
    // std::cout << '\n';
}

/* EXPECTED OUTPUT
block: a() b() ~b() ~a()
array: x0() x1() x2() ~x2() ~x1() ~x0()
class: base() filter() pump() body ~body ~pump() ~filter() ~base()
temporary: tmp() same-expression ~tmp() next-expression
*/
