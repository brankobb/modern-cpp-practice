// Rešenje zadatka ex1_predict_traits.

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

struct Nothing {
    std::string s;
};

struct WithDestructor {
    std::string s;
    ~WithDestructor() {}
};

struct WithCopy {
    std::string s;
    WithCopy() = default;
    WithCopy(const WithCopy& o) : s(o.s) {}
};

struct MoveOnly {
    std::unique_ptr<int> p;
    std::string s;
};

struct MoveWithoutNoexcept {
    std::string s;
    MoveWithoutNoexcept() = default;
    MoveWithoutNoexcept(MoveWithoutNoexcept&& o) : s(std::move(o.s)) {}
};

// Korak 1 i 2:
// Nothing: sve generisano, move stringa je noexcept.
static_assert(std::is_copy_constructible_v<Nothing>);
static_assert(std::is_move_constructible_v<Nothing>);
static_assert(std::is_nothrow_move_constructible_v<Nothing>);

// WithDestructor: korisnički destruktor -> move NIJE deklarisan. Trait
// "move constructible" je ipak tačan, jer T(std::move(x)) radi -- preko
// COPY konstruktora (const T& prima i rvalue). Kopiranje stringa alocira i
// može da baci, pa "nothrow move" nije tačno.
static_assert(std::is_copy_constructible_v<WithDestructor>);
static_assert(std::is_move_constructible_v<WithDestructor>);
static_assert(!std::is_nothrow_move_constructible_v<WithDestructor>);

// WithCopy: korisnički copy konstruktor -> isto, move ide preko kopije.
static_assert(std::is_copy_constructible_v<WithCopy>);
static_assert(std::is_move_constructible_v<WithCopy>);
static_assert(!std::is_nothrow_move_constructible_v<WithCopy>);

// MoveOnly: unique_ptr nema kopiju, pa ni klasa (generisana kopija je
// obrisana); generisani move je noexcept jer su move-ovi članova noexcept.
static_assert(!std::is_copy_constructible_v<MoveOnly>);
static_assert(std::is_move_constructible_v<MoveOnly>);
static_assert(std::is_nothrow_move_constructible_v<MoveOnly>);

// MoveWithoutNoexcept: korisnički move -> kopija obrisana; move postoji, ali
// bez noexcept kompajler mora da pretpostavi da može da baci.
static_assert(!std::is_copy_constructible_v<MoveWithoutNoexcept>);
static_assert(std::is_move_constructible_v<MoveWithoutNoexcept>);
static_assert(!std::is_nothrow_move_constructible_v<MoveWithoutNoexcept>);

template <typename T>
void row(const char* name) {
    std::cout << name << ": copy " << std::is_copy_constructible_v<T> << ", move "
              << std::is_move_constructible_v<T> << ", nothrow move "
              << std::is_nothrow_move_constructible_v<T> << '\n';
}

int main() {
    row<Nothing>("Nothing");
    row<WithDestructor>("WithDestructor");
    row<WithCopy>("WithCopy");
    row<MoveOnly>("MoveOnly");
    row<MoveWithoutNoexcept>("MoveWithoutNoexcept");
}
