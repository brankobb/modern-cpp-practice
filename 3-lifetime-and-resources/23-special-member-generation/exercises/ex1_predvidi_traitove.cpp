// KIND: usage
//
// Zadatak 1 -- pročitaj tabelu generisanja preko type traits (sekcija 1)
//   ./build.sh 3-lifetime-and-resources/23-special-member-generation/exercises/ex1_predvidi_traitove.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_predvidi_traitove.cpp
//
// Pet klasa, svaka sa std::string članom, razlikuju se po tome šta je
// korisnik DEKLARISAO.
// Korak 1: za svaku klasu PREDVIDI tri stvari i zapiši ih kao
//   static_assert ispod klase (<type_traits>, C++17 _v oblik):
//     std::is_copy_constructible_v<T>
//     std::is_move_constructible_v<T>
//     std::is_nothrow_move_constructible_v<T>
//   Kompajlira se samo ako je predviđanje tačno.
// Korak 2: zašto je is_move_constructible_v tačno za SaDestruktorom, a
//   is_nothrow_move_constructible_v netačno? (Hint: šta se zaista poziva
//   za T(std::move(x)), i da li kopiranje std::string-a sme da baci?)
// Korak 3: otkomentariši ispis tabele i uporedi.

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

struct Nista {
    std::string s;
};

struct SaDestruktorom {
    std::string s;
    ~SaDestruktorom() {}
};

struct SaKopijom {
    std::string s;
    SaKopijom() = default;
    SaKopijom(const SaKopijom& o) : s(o.s) {}
};

struct SamoMove {
    std::unique_ptr<int> p;
    std::string s;
};

struct MoveBezNoexcept {
    std::string s;
    MoveBezNoexcept() = default;
    MoveBezNoexcept(MoveBezNoexcept&& o) : s(std::move(o.s)) {}
};

// TODO korak 1: static_assert-ovi

template <typename T>
void red(const char* ime) {
    std::cout << ime << ": kopija " << std::is_copy_constructible_v<T> << ", move "
              << std::is_move_constructible_v<T> << ", nothrow move "
              << std::is_nothrow_move_constructible_v<T> << '\n';
}

int main() {
    // Korak 3 -- otkomentariši:
    // red<Nista>("Nista");
    // red<SaDestruktorom>("SaDestruktorom");
    // red<SaKopijom>("SaKopijom");
    // red<SamoMove>("SamoMove");
    // red<MoveBezNoexcept>("MoveBezNoexcept");
}

/* EXPECTED OUTPUT
Nista: kopija 1, move 1, nothrow move 1
SaDestruktorom: kopija 1, move 1, nothrow move 0
SaKopijom: kopija 1, move 1, nothrow move 0
SamoMove: kopija 0, move 1, nothrow move 1
MoveBezNoexcept: kopija 0, move 1, nothrow move 0
*/
