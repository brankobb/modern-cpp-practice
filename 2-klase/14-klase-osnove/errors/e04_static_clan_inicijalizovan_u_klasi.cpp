// STD: c++17
// EXPECT-GCC: ISO C++ forbids in-class initialization of non-const static member 'Counter::count'
// EXPECT-CLANG: non-const static data member must be initialized out of line
// POGREŠNO: "static int count = 0;" u klasi.
// Zašto: static član bez inline je u klasi samo DEKLARACIJA; definicija
//   (i vrednost) ide u tačno jedan .cpp. Izuzetak su const celobrojni i
//   constexpr članovi.
// Ispravno (C++17): "inline static int count = 0;" u klasi. Ili pre C++17:
//   "static int count;" u klasi + "int Counter::count = 0;" u jednom .cpp
//   (bez te linije linker javlja undefined reference, lekcija 08, errors/e11).
class Counter {
public:
    static int count = 0;
};

int main() {
    return Counter::count;
}
