// Rešenje zadatka ex3_lambda_reference.

#include <functional>
#include <iostream>

// Ako lambda koja se vraća (ili čuva za kasnije) hvata lokalne sa [&]
// (nije dobro): reference pokazuju na okvir funkcije koji više ne postoji.
// Treba ovako: stanje po vrednosti, u samoj lambdi. init-capture pravi
// novu promenljivu u objektu lambde; mutable dozvoljava da je menja.
std::function<int()> makeCounter() {
    return [state = 0]() mutable { return ++state; };
}

int main() {
    auto f = makeCounter();
    int first = f();
    int second = f();
    int third = f();
    std::cout << first << ' ' << second << ' ' << third << '\n';
}
