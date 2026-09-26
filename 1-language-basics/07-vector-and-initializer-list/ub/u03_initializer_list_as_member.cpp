// EXPECT-UB: stack-use-after-scope
// POGREŠNO: konstruktor sačuva std::initializer_list u član.
// Zašto: initializer_list je samo pokazivač + dužina na privremeni niz
//   (kao string_view). Niz za Config c{8080, 8081, 8082} živi dok traje
//   poziv konstruktora; posle toga ports_ pokazuje na nestali niz.
//   Zanimljivo: kod AGREGATA (struct Config { std::initializer_list<int>
//   ports; }; Config c{{8080, 8081}};) život niza se produži i kod je
//   ispravan (test). Kroz konstruktor -- ne. Ni g++ ni clang ne upozore.
// Ispravno: kopiraj u std::vector<int> u konstruktoru (main.cpp, Polygon).
#include <cstdio>
#include <initializer_list>

class Config {
public:
    Config(std::initializer_list<int> ports) : ports_(ports) {}
    int sum() const {
        int s = 0;
        for (int p : ports_) s += p;
        return s;
    }

private:
    std::initializer_list<int> ports_;
};

int main() {
    Config c{8080, 8081, 8082};
    std::printf("%d\n", c.sum());
}
