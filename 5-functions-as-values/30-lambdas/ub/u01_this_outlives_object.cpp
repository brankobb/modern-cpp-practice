// EXPECT-UB: heap-use-after-free
// UB: [=] u metodi NE kopira članove -- zarobi pokazivač this (u C++20 je
// taj implicitni capture zastareo, i oba kompajlera upozore). Lambda
// čita threshold_ kroz this, a objekat je već uništen: heap-use-after-free.
// "Po vrednosti" se odnosi na pokazivač, ne na objekat.
// Ispravno: [threshold = threshold_] (kopija samo onoga što treba) ili [*this]
// (C++17, kopija celog objekta) -- main.cpp, sekcija 7.
#include <functional>
#include <iostream>
#include <memory>
struct Sensor {
    int threshold_ = 10;
    std::function<bool(int)> makeCheck() const {
        return [=](int t) { return t > threshold_; };
    }
};
int main() {
    std::function<bool(int)> check;
    {
        auto s = std::make_unique<Sensor>();
        check = s->makeCheck();
    }   // Sensor uništen
    std::cout << check(20) << '\n';
}
