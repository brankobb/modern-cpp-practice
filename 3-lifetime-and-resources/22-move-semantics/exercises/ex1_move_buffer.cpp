// KIND: usage
//
// Zadatak 1 -- move konstruktor i move dodela (sekcije 4, 6)
//   ./build.sh 3-lifetime-and-resources/22-move-semantics/exercises/ex1_move_buffer.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_move_buffer.cpp
//
// Buffer drži int niz na heap-u i već ima kopiju (rule of 3). Globalni
// brojači broje kopije i pomeranja.
// Korak 1: Buffer(Buffer&& o) noexcept -- "ukradi" pokazivač i veličinu od
//   o, a o ostavi PRAZAN (nullptr, 0) -- tako njegov destruktor ne
//   oslobodi ukradeni blok. Uvećaj counter.moves.
// Korak 2: Buffer& operator=(Buffer&& o) noexcept -- oslobodi svoj blok,
//   preuzmi o-ov, o ostavi praznim. Pazi na a = std::move(a)
//   (proveri this != &o).
// Korak 3: u testu: std::move u vector, vraćanje iz funkcije, zamena
//   dva bafera preko tri move-a (kao std::swap). Na kraju proveri da
//   nijedna kopija nije napravljena.

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

struct Counter {
    int copies = 0;
    int moves = 0;
} counter;

class Buffer {
public:
    explicit Buffer(std::size_t n) : n_(n), p_(new int[n]()) {}
    ~Buffer() { delete[] p_; }
    Buffer(const Buffer& o) : n_(o.n_), p_(new int[o.n_]) {
        std::copy(o.p_, o.p_ + n_, p_);
        ++counter.copies;
    }
    Buffer& operator=(const Buffer& o) {
        Buffer tmp(o);
        std::swap(n_, tmp.n_);
        std::swap(p_, tmp.p_);
        return *this;
    }

    // TODO korak 1 i 2

    std::size_t size() const { return n_; }

private:
    std::size_t n_;
    int* p_;
};

Buffer makeBuffer(std::size_t n) {
    Buffer b(n);
    return b;           // NRVO ili automatski move -- nikad kopija (lekcija 24)
}

int main() {
    // Korak 3 -- otkomentariši (posle koraka 1 i 2):
    // std::vector<Buffer> v;
    // v.reserve(2);
    // Buffer a(100);
    // v.push_back(std::move(a));
    // std::cout << "a after move: " << a.size() << ", in vector: " << v[0].size() << '\n';
    // v.push_back(makeBuffer(50));
    // Buffer x(1), y(2);
    // Buffer t = std::move(x);
    // x = std::move(y);
    // y = std::move(t);
    // std::cout << "x: " << x.size() << ", y: " << y.size() << '\n';
    // Buffer& same = x;
    // x = std::move(same);                  // mora da preživi
    // std::cout << "x after x = move(x): " << x.size() << '\n';
    // std::cout << "copies: " << counter.copies << '\n';
}

/* EXPECTED OUTPUT
a after move: 0, in vector: 100
x: 2, y: 1
x after x = move(x): 2
copies: 0
*/
