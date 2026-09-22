// EXPECT-UB: stack-overflow
// POGREŠNO: move dodela napisana kao std::swap(*this, other).
// Zašto: std::swap(x, y) je "tmp = move(x); x = move(y); y = move(tmp)" --
//   zove move DODELU. Naša move dodela zove std::swap, koji zove move
//   dodelu... beskonačna rekurzija dok se stek ne prepuni. Nijedan kompajler
//   ne upozori, jer rekurzija ide kroz šablon iz biblioteke.
// Ispravno: swap koji menja ČLANOVE (friend void swap(Buffer& a, Buffer& b)
//   { std::swap(a.data_, b.data_); ... }), pa move dodela sme da zove taj
//   swap (main.cpp, sekcija 1 i 5).
#include <cstdio>
#include <utility>

class Buffer {
public:
    explicit Buffer(int n) : data_(new int[n]{}), size_(n) {}
    Buffer(Buffer&& o) noexcept : data_(std::exchange(o.data_, nullptr)), size_(std::exchange(o.size_, 0)) {}
    Buffer& operator=(Buffer&& o) noexcept {
        std::swap(*this, o);
        return *this;
    }
    ~Buffer() { delete[] data_; }
    int size() const { return size_; }

private:
    int* data_;
    int size_;
};

int main() {
    Buffer a(4);
    Buffer b(8);
    a = std::move(b);
    std::printf("%d\n", a.size());
}
