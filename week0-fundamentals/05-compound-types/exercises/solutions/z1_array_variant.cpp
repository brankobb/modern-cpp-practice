// Rešenje zadatka z1_array_variant.

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>

// Korak 1: std::array se prosleđuje kao i svaki objekat (const&), i NE
// raspada se u pokazivač -- veličina je deo tipa.
double prosek(const std::array<int, 5>& a) {
    int s = 0;
    for (int x : a) s += x;
    return static_cast<double>(s) / static_cast<double>(a.size());
}

// Korak 2: variant zna koji tip trenutno drži. get_if je provera i pristup
// u jednom koraku; std::get<T> bi bacio std::bad_variant_access za pogrešan tip.
using Poruka = std::variant<int, double, std::string>;

void opisi(const Poruka& p) {
    if (const int* i = std::get_if<int>(&p))
        std::cout << "int " << *i << '\n';
    else if (const double* d = std::get_if<double>(&p))
        std::cout << "double " << *d << '\n';
    else if (const std::string* s = std::get_if<std::string>(&p))
        std::cout << "string " << *s << '\n';
}

// Korak 3: using je čitljiviji od typedef int (*Obrada)(int); (EMC Item 9).
using Obrada = int (*)(int);

int udvostruci(int x) { return 2 * x; }
int dodajJedan(int x) { return x + 1; }

int primeni(const std::array<Obrada, 2>& koraci, int x) {
    for (Obrada f : koraci) x = f(x);
    return x;
}

int main() {
    std::array<int, 5> ocitavanja{10, 20, 30, 40, 50};
    std::cout << "broj: " << ocitavanja.size() << " prvi: " << ocitavanja.front()
              << " poslednji: " << ocitavanja.back() << '\n';
    std::cout << "prosek: " << prosek(ocitavanja) << '\n';
    try {
        std::cout << ocitavanja.at(5);
    } catch (const std::out_of_range&) {
        std::cout << "at(5): out_of_range\n";
    }

    for (const Poruka& p : {Poruka{42}, Poruka{3.5}, Poruka{std::string("zdravo")}})
        opisi(p);

    std::array<Obrada, 2> koraci{udvostruci, dodajJedan};
    std::cout << "10 -> " << primeni(koraci, 10) << '\n';
}
