// EXPECT-GCC: no match for call to
// EXPECT-CLANG: no matching function for call to object of type
// POGREŠNO: bind izraz koristi _1 i _2, pa traži bar dva argumenta pri
// pozivu. Greška je duboko u <functional>, sa tipom od nekoliko redova --
// uporedi sa porukom za lambdu kojoj fali argument (kratka i jasna).
// (Obrnuto, VIŠAK argumenata bind tiho ignoriše -- main.cpp, sekcija 5.)
// Ispravno: f(3, 4), ili lambda [](int a, int b) { return times(a, b); }.
#include <functional>
int times(int a, int b) { return a * b; }
int main() {
    using namespace std::placeholders;
    auto f = std::bind(times, _1, _2);
    return f(3);
}
