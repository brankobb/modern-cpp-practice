// Rešenje zadatka ex1_move_buffer.

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

    // Korak 1: bez alokacije -- samo preuzmi pokazivač. std::exchange
    // vrati staru vrednost i upiše novu, u jednom koraku.
    // noexcept: vector tada pri rastu pomera umesto da kopira (lekcija 23).
    Buffer(Buffer&& o) noexcept : n_(std::exchange(o.n_, 0)), p_(std::exchange(o.p_, nullptr)) {
        ++counter.moves;
    }

    // Korak 2: bez provere, a = std::move(a) bi oslobodio sopstveni blok,
    // pa ga "preuzeo" -- visi pokazivač.
    Buffer& operator=(Buffer&& o) noexcept {
        if (this != &o) {
            delete[] p_;
            n_ = std::exchange(o.n_, 0);
            p_ = std::exchange(o.p_, nullptr);
            ++counter.moves;
        }
        return *this;
    }

    std::size_t size() const { return n_; }

private:
    std::size_t n_;
    int* p_;
};

Buffer makeBuffer(std::size_t n) {
    Buffer b(n);
    return b;
}

int main() {
    std::vector<Buffer> v;
    v.reserve(2);
    Buffer a(100);
    v.push_back(std::move(a));
    // Moved-from objekat je validan (sme da se uništi, dodeli, pita za
    // veličinu), samo je prazan -- tako smo ga napisali.
    std::cout << "a after move: " << a.size() << ", in vector: " << v[0].size() << '\n';
    v.push_back(makeBuffer(50));
    Buffer x(1), y(2);
    Buffer t = std::move(x);
    x = std::move(y);
    y = std::move(t);
    std::cout << "x: " << x.size() << ", y: " << y.size() << '\n';
    Buffer& same = x;
    x = std::move(same);
    std::cout << "x after x = move(x): " << x.size() << '\n';
    std::cout << "copies: " << counter.copies << '\n';
}
