// Rešenje zadatka ex1_ring_buffer.

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>

template <typename T, std::size_t N>
class RingBuffer {
    static_assert(N > 0, "RingBuffer: capacity must be > 0");

public:
    // Korak 1: indeks sledećeg mesta za upis je (head_ + count_) % N.
    void push(const T& v) {
        data_[(head_ + count_) % N] = v;
        if (count_ < N)
            ++count_;
        else
            head_ = (head_ + 1) % N;   // pun: upravo je pregažen najstariji
    }

    T pop() {
        if (count_ == 0) throw std::underflow_error("RingBuffer is empty");
        T v = data_[head_];
        head_ = (head_ + 1) % N;
        --count_;
        return v;
    }

    std::size_t size() const { return count_; }
    bool full() const { return count_ == N; }
    static constexpr std::size_t capacity() { return N; }

    // Korak 2: fold preko zareza -- push za svaki argument, redom sleva
    // nadesno. static_cast<T>: argumenti mogu biti drugog tipa (int -> uint8_t).
    template <typename... Args>
    void pushAll(const Args&... args) {
        (push(static_cast<T>(args)), ...);
    }

private:
    std::array<T, N> data_{};
    std::size_t head_ = 0;
    std::size_t count_ = 0;
};

// Korak 3: alias šablon -- nov naziv, isti tip.
template <std::size_t N>
using ByteBuffer = RingBuffer<std::uint8_t, N>;

int main() {
    std::cout << std::boolalpha;
    RingBuffer<int, 4> b;
    b.pushAll(1, 2, 3, 4, 5);
    std::cout << "size " << b.size() << "/" << b.capacity() << ", full: " << b.full() << '\n';
    std::cout << "pop:";
    while (b.size() > 0) std::cout << ' ' << b.pop();
    std::cout << '\n';
    try {
        b.pop();
    } catch (const std::underflow_error&) {
        std::cout << "empty pop: underflow_error\n";
    }

    ByteBuffer<3> bb;
    bb.pushAll(10, 20, 30, 40);
    std::cout << "ByteBuffer<3>:";
    while (bb.size() > 0) std::cout << ' ' << int(bb.pop());
    std::cout << '\n';
}
