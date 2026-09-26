// Rešenje zadatka ex1_predvidi_traitove.

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

// Korak 1 i 2:
// Nista: sve generisano, move stringa je noexcept.
static_assert(std::is_copy_constructible_v<Nista>);
static_assert(std::is_move_constructible_v<Nista>);
static_assert(std::is_nothrow_move_constructible_v<Nista>);

// SaDestruktorom: korisnički destruktor -> move NIJE deklarisan. Trait
// "move constructible" je ipak tačan, jer T(std::move(x)) radi -- preko
// COPY konstruktora (const T& prima i rvalue). Kopiranje stringa alocira i
// može da baci, pa "nothrow move" nije tačno.
static_assert(std::is_copy_constructible_v<SaDestruktorom>);
static_assert(std::is_move_constructible_v<SaDestruktorom>);
static_assert(!std::is_nothrow_move_constructible_v<SaDestruktorom>);

// SaKopijom: korisnički copy konstruktor -> isto, move ide preko kopije.
static_assert(std::is_copy_constructible_v<SaKopijom>);
static_assert(std::is_move_constructible_v<SaKopijom>);
static_assert(!std::is_nothrow_move_constructible_v<SaKopijom>);

// SamoMove: unique_ptr nema kopiju, pa ni klasa (generisana kopija je
// obrisana); generisani move je noexcept jer su move-ovi članova noexcept.
static_assert(!std::is_copy_constructible_v<SamoMove>);
static_assert(std::is_move_constructible_v<SamoMove>);
static_assert(std::is_nothrow_move_constructible_v<SamoMove>);

// MoveBezNoexcept: korisnički move -> kopija obrisana; move postoji, ali
// bez noexcept kompajler mora da pretpostavi da može da baci.
static_assert(!std::is_copy_constructible_v<MoveBezNoexcept>);
static_assert(std::is_move_constructible_v<MoveBezNoexcept>);
static_assert(!std::is_nothrow_move_constructible_v<MoveBezNoexcept>);

template <typename T>
void red(const char* ime) {
    std::cout << ime << ": kopija " << std::is_copy_constructible_v<T> << ", move "
              << std::is_move_constructible_v<T> << ", nothrow move "
              << std::is_nothrow_move_constructible_v<T> << '\n';
}

int main() {
    red<Nista>("Nista");
    red<SaDestruktorom>("SaDestruktorom");
    red<SaKopijom>("SaKopijom");
    red<SamoMove>("SamoMove");
    red<MoveBezNoexcept>("MoveBezNoexcept");
}
