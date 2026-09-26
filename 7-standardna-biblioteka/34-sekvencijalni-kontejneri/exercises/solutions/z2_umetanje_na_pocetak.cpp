// Rešenje zadatka z2_umetanje_na_pocetak.

#include <deque>
#include <iostream>
#include <vector>

int pomeranja = 0;

struct Poruka {
    int id;
    explicit Poruka(int i) : id(i) {}
    Poruka(const Poruka& o) : id(o.id) { ++pomeranja; }
    Poruka(Poruka&& o) noexcept : id(o.id) { ++pomeranja; }
    Poruka& operator=(const Poruka& o) {
        id = o.id;
        ++pomeranja;
        return *this;
    }
    Poruka& operator=(Poruka&& o) noexcept {
        id = o.id;
        ++pomeranja;
        return *this;
    }
};

int main() {
    // Ako često umećeš na početak vektora (nije dobro): svako umetanje
    // pomeri sve elemente -- O(n) po umetanju, O(n^2) ukupno.
    // Treba ovako: deque -- dodavanje na oba kraja je O(1), postojeći
    // elementi ostaju gde jesu. emplace_front pravi element na mestu.
    std::deque<Poruka> red;
    for (int i = 0; i < 100; ++i) red.emplace_front(i);
    std::cout << "prvi: " << red.front().id << ", poslednji: " << red.back().id << '\n';
    std::cout << "pomeranja elemenata: " << pomeranja << '\n';
}
