// Rešenje zadatka z1_kruzni_bafer.

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>

template <typename T, std::size_t N>
class KruzniBafer {
    static_assert(N > 0, "KruzniBafer: kapacitet mora biti > 0");

public:
    // Korak 1: indeks sledećeg mesta za upis je (glava_ + broj_) % N.
    void push(const T& v) {
        podaci_[(glava_ + broj_) % N] = v;
        if (broj_ < N)
            ++broj_;
        else
            glava_ = (glava_ + 1) % N;   // pun: upravo je pregažen najstariji
    }

    T pop() {
        if (broj_ == 0) throw std::underflow_error("KruzniBafer je prazan");
        T v = podaci_[glava_];
        glava_ = (glava_ + 1) % N;
        --broj_;
        return v;
    }

    std::size_t velicina() const { return broj_; }
    bool pun() const { return broj_ == N; }
    static constexpr std::size_t kapacitet() { return N; }

    // Korak 2: fold preko zareza -- push za svaki argument, redom sleva
    // nadesno. static_cast<T>: argumenti mogu biti drugog tipa (int -> uint8_t).
    template <typename... Args>
    void dodajSve(const Args&... args) {
        (push(static_cast<T>(args)), ...);
    }

private:
    std::array<T, N> podaci_{};
    std::size_t glava_ = 0;
    std::size_t broj_ = 0;
};

// Korak 3: alias šablon -- nov naziv, isti tip.
template <std::size_t N>
using BajtBafer = KruzniBafer<std::uint8_t, N>;

int main() {
    std::cout << std::boolalpha;
    KruzniBafer<int, 4> b;
    b.dodajSve(1, 2, 3, 4, 5);
    std::cout << "velicina " << b.velicina() << "/" << b.kapacitet() << ", pun: " << b.pun() << '\n';
    std::cout << "pop:";
    while (b.velicina() > 0) std::cout << ' ' << b.pop();
    std::cout << '\n';
    try {
        b.pop();
    } catch (const std::underflow_error&) {
        std::cout << "prazan pop: underflow_error\n";
    }

    BajtBafer<3> bb;
    bb.dodajSve(10, 20, 30, 40);
    std::cout << "BajtBafer<3>:";
    while (bb.velicina() > 0) std::cout << ' ' << int(bb.pop());
    std::cout << '\n';
}
