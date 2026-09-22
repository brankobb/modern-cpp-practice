// STD: c++17
// EXPECT-GCC: as 'this' argument discards qualifiers
// EXPECT-CLANG: but function is not marked const
// POGREŠNO: na const objektu mogu da se zovu samo const member funkcije.
// U const objektu je "this" tipa const Counter*, a increment() traži Counter*.
// Ispravno: označi funkciju const ako ne menja objekat; increment() menja, pa
// se na const objektu jednostavno ne sme zvati.
struct Counter {
    int value = 0;
    void increment() { ++value; }
};
int main() {
    const Counter c{};
    c.increment();
}
