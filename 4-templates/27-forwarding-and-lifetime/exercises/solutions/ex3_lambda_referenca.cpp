// Rešenje zadatka ex3_lambda_referenca.

#include <functional>
#include <iostream>

// Ako lambda koja se vraća (ili čuva za kasnije) hvata lokalne sa [&]
// (nije dobro): reference pokazuju na okvir funkcije koji više ne postoji.
// Treba ovako: stanje po vrednosti, u samoj lambdi. init-capture pravi
// novu promenljivu u objektu lambde; mutable dozvoljava da je menja.
std::function<int()> napraviBrojac() {
    return [stanje = 0]() mutable { return ++stanje; };
}

int main() {
    auto f = napraviBrojac();
    int prvi = f();
    int drugi = f();
    int treci = f();
    std::cout << prvi << ' ' << drugi << ' ' << treci << '\n';
}
