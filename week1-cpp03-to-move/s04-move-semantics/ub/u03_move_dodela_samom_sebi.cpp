// EXPECT-UB: heap-use-after-free
// POGREŠNO: move dodela koja prvo obriše sopstvenu memoriju, pa preuzme
//   tuđu -- a "tuđa" je ista (a = std::move(a)).
// Zašto: delete[] data_ oslobodi niz; std::exchange(o.data_, nullptr) vrati
//   isti (sada oslobođen) pokazivač i upiše ga nazad u data_. a.first()
//   čita oslobođenu memoriju. Dodela samom sebi preko move-a je retka, ali
//   se desi u algoritmima (std::swap(x, x), std::shuffle).
// Ispravno: if (this != &other) oko svega (main.cpp, sekcija 4), ili
//   "preuzmi u lokalni objekat pa swap".
#include <cstdio>
#include <utility>

class Buffer {
public:
    explicit Buffer(int n) : data_(new int[n]{}), size_(n) {}
    Buffer(Buffer&& o) noexcept : data_(std::exchange(o.data_, nullptr)), size_(std::exchange(o.size_, 0)) {}
    Buffer& operator=(Buffer&& o) noexcept {
        delete[] data_;
        data_ = std::exchange(o.data_, nullptr);
        size_ = std::exchange(o.size_, 0);
        return *this;
    }
    ~Buffer() { delete[] data_; }
    int first() const { return data_[0]; }

private:
    int* data_;
    int size_;
};

int main() {
    Buffer a(4);
    Buffer& alias = a;
    a = std::move(alias);
    std::printf("%d\n", a.first());
}
