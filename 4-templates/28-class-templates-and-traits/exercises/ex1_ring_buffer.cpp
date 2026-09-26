// KIND: usage
//
// Zadatak 1 -- klasni šablon sa ne-tipskim parametrom, variadic metoda sa
// fold izrazom, static_assert i alias šablon (sekcije 2, 3, 6, 8)
//   ./build.sh 4-templates/28-class-templates-and-traits/exercises/ex1_ring_buffer.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_ring_buffer.cpp
//
// Kružni bafer (ring buffer) je čest u embedded kodu: fiksan kapacitet,
// bez heap-a, a kad je pun, novi element pregazi najstariji.
// Korak 1: template <typename T, std::size_t N> class RingBuffer --
//   std::array<T, N> data_, indeks najstarijeg (head_) i broj elemenata.
//   void push(const T& v): kad je pun, pregazi najstariji (i pomeri glavu).
//   T pop(): vrati i ukloni najstariji; za prazan baci std::underflow_error.
//   std::size_t velicina() const, bool pun() const,
//   static constexpr std::size_t kapacitet().
//   static_assert(N > 0, "...") u klasi.
// Korak 2: template <typename... Args> void pushAll(const Args&... args)
//   -- push za svaki argument, fold izrazom preko zareza (bez petlje i
//   rekurzije).
// Korak 3: alias šablon template <std::size_t N> using ByteBuffer =
//   RingBuffer<std::uint8_t, N>;

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>

// TODO korak 1, 2, 3

int main() {
    std::cout << std::boolalpha;
    // Korak 1 i 2 -- otkomentariši:
    // RingBuffer<int, 4> b;
    // b.pushAll(1, 2, 3, 4, 5);
    // std::cout << "size " << b.size() << "/" << b.capacity() << ", full: " << b.full() << '\n';
    // std::cout << "pop:";
    // while (b.size() > 0) std::cout << ' ' << b.pop();
    // std::cout << '\n';
    // try {
    //     b.pop();
    // } catch (const std::underflow_error&) {
    //     std::cout << "empty pop: underflow_error\n";
    // }

    // Korak 3 -- otkomentariši:
    // ByteBuffer<3> bb;
    // bb.pushAll(10, 20, 30, 40);
    // std::cout << "ByteBuffer<3>:";
    // while (bb.size() > 0) std::cout << ' ' << int(bb.pop());
    // std::cout << '\n';
}

/* EXPECTED OUTPUT
size 4/4, full: true
pop: 2 3 4 5
empty pop: underflow_error
ByteBuffer<3>: 20 30 40
*/
