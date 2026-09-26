// EXPECT-GCC: std::thread arguments must be invocable after conversion to rvalues
// EXPECT-CLANG: '__is_invocable<void (Sensor::*)()>::value'
// POGREŠNO: metoda se poziva NA objektu. &Sensor::read je pokazivač na
// metodu; bez objekta nema koga da pozove.
// Ispravno: objekat (pokazivač ili std::ref) je prvi argument posle
// metode: std::thread t(&Sensor::read, &s);
// ili lambda: std::thread t([&s] { s.read(); });
#include <thread>
struct Sensor {
    int value = 0;
    void read() { value = 42; }
};
int main() {
    Sensor s;
    std::thread t(&Sensor::read);
    t.join();
    return s.value;
}
