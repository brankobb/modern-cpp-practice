// EXPECT-GCC: std::thread arguments must be invocable after conversion to rvalues
// EXPECT-CLANG: '__is_invocable<void (Senzor::*)()>::value'
// POGREŠNO: metoda se poziva NA objektu. &Senzor::citaj je pokazivač na
// metodu; bez objekta nema koga da pozove.
// Ispravno: objekat (pokazivač ili std::ref) je prvi argument posle
// metode: std::thread t(&Senzor::citaj, &s);
// ili lambda: std::thread t([&s] { s.citaj(); });
#include <thread>
struct Senzor {
    int vrednost = 0;
    void citaj() { vrednost = 42; }
};
int main() {
    Senzor s;
    std::thread t(&Senzor::citaj);
    t.join();
    return s.vrednost;
}
