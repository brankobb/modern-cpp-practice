// STD: c++17
// EXPECT-GCC: no matching function for call to 'Sensor::Sensor()'
// EXPECT-CLANG: no matching constructor for initialization of 'Sensor'
// POGREŠNO: resize(n) za tip bez podrazumevanog konstruktora.
// Zašto: resize(n) pravi nove elemente PODRAZUMEVANIM konstruktorom (isto
//   i vector<T>(n)). Sensor ga nema (ima samo explicit Sensor(int)).
// Ispravno: resize(n, Sensor(0)) (kopije zadatog), ili reserve(n) pa
//   emplace_back(id) za svaki element.
#include <vector>

struct Sensor {
    explicit Sensor(int id) : id_(id) {}
    int id_;
};

int main() {
    std::vector<Sensor> v;
    v.resize(3);
    return static_cast<int>(v.size());
}
