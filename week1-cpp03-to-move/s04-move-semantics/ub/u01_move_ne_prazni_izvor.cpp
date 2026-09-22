// EXPECT-UB: attempting double-free
// POGREŠNO: move konstruktor preuzme pokazivač, ali izvor ne postavi na nullptr.
// Zašto: posle "move-a" i a i b pokazuju na istu memoriju. To je plitka
//   kopija sa drugim imenom (s02). Na kraju main-a oba destruktora obrišu
//   isti niz.
// Ispravno: data_(std::exchange(other.data_, nullptr)) -- uzmi vrednost i
//   u istom koraku isprazni izvor (main.cpp, sekcija 4). Moved-from objekat
//   mora ostati ispravan: destruktor mora smeti da se pozove.
#include <cstdio>
#include <utility>

class Buffer {
public:
    explicit Buffer(int n) : data_(new int[n]{}), size_(n) {}
    Buffer(Buffer&& other) noexcept : data_(other.data_), size_(other.size_) {}
    ~Buffer() { delete[] data_; }
    int size() const { return size_; }

private:
    int* data_;
    int size_;
};

int main() {
    Buffer a(4);
    Buffer b = std::move(a);
    std::printf("%d\n", b.size());
}
