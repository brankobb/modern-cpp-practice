// EXPECT-UB: LeakSanitizer: detected memory leaks
// POGREŠNO: move dodela preuzme tuđu memoriju, a svoju staru ne obriše.
// Zašto: a već ima 64 int-a. Posle a = std::move(b) data_ pokazuje na b-ov
//   niz, a na stari niz više ne pokazuje niko: curenje od 256 bajtova po
//   dodeli (test: 10 dodela, 2560 bajtova).
// Ispravno: delete[] data_; pre preuzimanja (main.cpp, sekcija 1), ili
//   "preuzmi u lokalni objekat pa swap", pa stari niz ode sa lokalnim.
#include <cstdio>
#include <utility>

class Buffer {
public:
    explicit Buffer(int n) : data_(new int[n]{}), size_(n) {}
    Buffer(Buffer&& o) noexcept : data_(std::exchange(o.data_, nullptr)), size_(std::exchange(o.size_, 0)) {}
    Buffer& operator=(Buffer&& o) noexcept {
        if (this != &o) {
            data_ = std::exchange(o.data_, nullptr);
            size_ = std::exchange(o.size_, 0);
        }
        return *this;
    }
    ~Buffer() { delete[] data_; }
    int size() const { return size_; }

private:
    int* data_;
    int size_;
};

int main() {
    for (int i = 0; i < 10; ++i) {
        Buffer a(64);
        Buffer b(32);
        a = std::move(b);
        std::printf("%d ", a.size());
    }
    std::printf("\n");
}
