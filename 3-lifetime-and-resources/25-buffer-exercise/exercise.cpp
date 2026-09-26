#include <algorithm>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

// VEŽBA -- piši sam, pa uporedi sa main.cpp (rešenje).
//   ./build.sh 3-lifetime-and-resources/25-buffer-exercise/exercise.cpp
// Fajl se kompajlira i ovakav; testovi u main() se otključavaju kako
// dodaješ funkcije. Pokreći pod ASan-om posle svakog koraka.
//
// Korak 1 -- rule of 3:
//   - copy konstruktor: NOVA memorija + kopija sadržaja (lekcija 20)
//   - friend void swap(Buffer&, Buffer&) noexcept
//   - copy dodela preko copy-and-swap (lekcija 21), mora da radi i za a = a
// Korak 2 -- rule of 5:
//   - move konstruktor noexcept: std::exchange(other.data_, nullptr) (lekcija 22)
//   - move dodela noexcept: mora da radi i za a = std::move(a)
// Korak 3 -- provere:
//   - svaka funkcija uveća svoj brojač (copies / moves), pa uporedi brojeve
//     sa očekivanim u komentarima
//   - std::vector<Buffer> sa 5 push_back: posle koraka 2 realokacija ne
//     sme da kopira (copies ostaje 0). Probaj bez noexcept na move-u!
// Korak 4 -- rule of 0:
//   - napiši struct Buffer0 { std::vector<int> data; }; i proveri da radi
//     SVE ovo bez ijedne specijalne funkcije.

class Buffer {
public:
    explicit Buffer(std::size_t size) : data_(new int[size]{}), size_(size) {}
    ~Buffer() { delete[] data_; }

    // TODO korak 1: Buffer(const Buffer& other);
    // TODO korak 1: friend void swap(Buffer& a, Buffer& b) noexcept;
    // TODO korak 1: Buffer& operator=(const Buffer& other);
    // TODO korak 2: Buffer(Buffer&& other) noexcept;
    // TODO korak 2: Buffer& operator=(Buffer&& other) noexcept;

    // Dok nisi napisao korak 1, kopija je ZABRANJENA -- inače bi
    // kompajlerova plitka kopija dala double free (lekcija 20, ub/u01).
    // Obriši ove dve linije kad napišeš svoje.
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    std::size_t size() const { return size_; }
    int& operator[](std::size_t i) { return data_[i]; }

    inline static int copies = 0;
    inline static int moves = 0;

private:
    int* data_;
    std::size_t size_;
};

int main() {
    Buffer a(10);
    a[0] = 42;
    std::cout << "a.size()=" << a.size() << " a[0]=" << a[0] << "\n";

    // Korak 1 -- otkomentariši:
    // Buffer b = a;                // očekivano: copies=1
    // b[0] = 7;                    // a[0] mora ostati 42
    // Buffer& alias = a;
    // a = alias;                   // dodela samom sebi: ne sme da pukne
    // Korak 2 -- otkomentariši:
    // Buffer c = std::move(a);     // očekivano: moves=1, a.size()=0
    // c = Buffer(3);               // move dodela
    // std::vector<Buffer> v;
    // for (int i = 0; i < 5; ++i) v.push_back(Buffer(4));  // copies ne sme da poraste
    std::cout << "copies=" << Buffer::copies << " moves=" << Buffer::moves << "\n";
}
