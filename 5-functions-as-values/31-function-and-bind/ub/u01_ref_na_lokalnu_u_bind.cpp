// EXPECT-UB: stack-use-after-return
// UB: std::ref(brojac) u bind-u čuva REFERENCU na lokalnu promenljivu
// funkcije napraviBrojac(). Bind objekat se vrati (u std::function), a
// brojac nestane sa okvirom funkcije. Poziv upisuje u mrtav okvir steka.
// (Isto kao [&] u lambdi koja nadživi funkciju -- lekcije 27, 30.)
// Ispravno: stanje u samom objektu -- lambda [n = 0]() mutable { return ++n; }.
#include <functional>
#include <iostream>
int uvecaj(int& n) { return ++n; }
std::function<int()> napraviBrojac() {
    int brojac = 0;
    return std::bind(uvecaj, std::ref(brojac));
}
int main() {
    auto f = napraviBrojac();
    std::cout << f() << '\n';
}
