// EXPECT-UB: heap-use-after-free
// UB: [=] u metodi NE kopira članove -- zarobi pokazivač this (u C++20 je
// taj implicitni capture zastareo, i oba kompajlera upozore). Lambda
// čita prag_ kroz this, a objekat je već uništen: heap-use-after-free.
// "Po vrednosti" se odnosi na pokazivač, ne na objekat.
// Ispravno: [prag = prag_] (kopija samo onoga što treba) ili [*this]
// (C++17, kopija celog objekta) -- main.cpp, sekcija 7.
#include <functional>
#include <iostream>
#include <memory>
struct Senzor {
    int prag_ = 10;
    std::function<bool(int)> napraviProveru() const {
        return [=](int t) { return t > prag_; };
    }
};
int main() {
    std::function<bool(int)> provera;
    {
        auto s = std::make_unique<Senzor>();
        provera = s->napraviProveru();
    }   // Senzor uništen
    std::cout << provera(20) << '\n';
}
