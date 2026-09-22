// STD: c++17
// LINK: support/empty.cpp
// EXPECT-GCC: undefined reference to `Counter::count'
// EXPECT-CLANG: undefined reference to `Counter::count'
// POGREŠNO: static član klase je samo deklarisan u klasi.
// Zašto: "static int count;" u klasi je DEKLARACIJA. Pre C++17 je definicija
//   morala da bude u tačno jednom .cpp ("int Counter::count = 0;"). Bez nje
//   se sve kompajlira, a linker ne nađe simbol.
// Ispravno (C++17): "inline static int count = 0;" u klasi. static constexpr
//   članovi su od C++17 implicitno inline, pa njima definicija van klase ne
//   treba (main.cpp, sekcija 5).
struct Counter {
    static int count;
};

int main() {
    return ++Counter::count;
}
