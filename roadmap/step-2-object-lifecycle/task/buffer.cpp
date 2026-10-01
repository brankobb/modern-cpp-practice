// Korak 2 -- ZADATAK: "Buffer: Rule of 5 od nule"
//   ./build.sh roadmap/step-2-object-lifecycle/task/buffer.cpp
// Uputstvo: roadmap/step-2-object-lifecycle/notes.md, deo "Zadatak".
// Rešenje: ../solutions/buffer.cpp (otvori tek kad tvoj izlaz bude isti kao
// blok EXPECTED OUTPUT na dnu fajla).
//
// Fajl se kompajlira i ovakav. Piši klasu Buffer, pa otkomentariši testove
// u main()-u jedan po jedan.
//
// Klasa Buffer:
//   članovi (TIM redom -- to je i redosled inicijalizacije):
//     int id_;              -- ++next_id_ u SVAKOM konstruktoru (ne kopira se, ne pomera se)
//     std::size_t size_;
//     std::uint8_t* data_;  -- new std::uint8_t[size], delete[] u destruktoru
//     static inline int next_id_ = 0;
//   explicit Buffer(std::size_t size, std::uint8_t fill = 0)
//       ispis: "  #<id> ctor (<size> bytes)"
//   ~Buffer()
//       ispis: "  #<id> dtor (<size> bytes)" ili, ako je data_ == nullptr,
//              "  #<id> dtor (<size> bytes, moved-from)"
//   Buffer(const Buffer&)             "  #<id> copy ctor from #<other id> (<size> bytes)"
//   Buffer& operator=(const Buffer&)  "  #<id> copy assign from #<other id> (<other size> bytes)"
//   Buffer(Buffer&&) noexcept         "  #<id> move ctor from #<other id> (<size> bytes)"
//   Buffer& operator=(Buffer&&) noexcept
//                                     "  #<id> move assign from #<other id> (<other size> bytes)"
//   (ispis u dodelama ide PRE same dodele)
//   size(), data() (const i ne-const verzija), id()
//
// Pre nego što napišeš ispravnu copy dodelu, napiši NAIVNU (delete[] pa
// new[] pa kopiraj) i pogledaj šta test 6 ispiše. Objasni sebi zašto.
// Pomoć: std::copy_n i std::fill_n (<algorithm>), std::exchange (<utility>).

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <utility>

// TODO: class Buffer

// Otkomentariši kad Buffer postoji:
// bool same_content(const Buffer& a, const Buffer& b) {
//     return a.size() == b.size() && std::equal(a.data(), a.data() + a.size(), b.data());
// }

void check(bool ok, const char* what) { std::printf("  %s: %s\n", ok ? "ok" : "FAIL", what); }

int main() {
    // Test 1 -- otkomentariši:
    // std::printf("== 1. Buffer a(10)\n");
    // Buffer a(10, 0xAB);
    // a.data()[0] = 1;
    // const Buffer original = a;               // za kasnija poređenja (i ovo je copy ctor)

    // Test 2 -- otkomentariši:
    // std::printf("== 2. Buffer b = a  -> copy ctor\n");
    // Buffer b = a;
    // check(b.data() != a.data(), "b.data() != a.data()  (own memory)");
    // check(same_content(a, b), "same content");
    // b.data()[1] = 7;
    // check(a.data()[1] == 0xAB, "changing b does not change a");

    // Test 3 -- otkomentariši:
    // std::printf("== 3. Buffer c(5); c = a  -> copy assignment\n");
    // Buffer c(5);
    // c = a;
    // check(c.size() == 10 && same_content(c, a), "c is now a copy of a");

    // Test 4 -- otkomentariši:
    // std::printf("== 4. Buffer d = std::move(a)  -> move ctor\n");
    // const std::uint8_t* a_memory = a.data();
    // Buffer d = std::move(a);
    // check(a.data() == nullptr && a.size() == 0, "a is empty (valid moved-from state)");
    // check(d.data() == a_memory, "d took a's memory, no copy");
    // check(same_content(d, original), "d has the original content");

    // Test 5 -- otkomentariši:
    // std::printf("== 5. Buffer e(3); e = std::move(b)  -> move assignment\n");
    // Buffer e(3);
    // e = std::move(b);
    // check(b.data() == nullptr && e.size() == 10 && e.data()[1] == 7, "e took b's memory");

    // Test 6 -- otkomentariši:
    // std::printf("== 6. e = e  -> self-assignment\n");
    // Buffer& alias = e;                       // realan slučaj: v[i] = v[j] kad je i == j
    // e = alias;                               // (direktno "e = e;" clang odbije sa -Wself-assign-overloaded)
    // check(e.size() == 10 && e.data()[1] == 7, "e survived self-assignment");

    // Test 7 -- otkomentariši:
    // std::printf("== 7. moved-from object is reusable\n");
    // a = d;                                   // dodela u moved-from objekat mora da radi
    // check(same_content(a, d), "a = d after a was moved from");

    std::printf("== end of main: destructors in reverse order\n");
}

/* EXPECTED OUTPUT
== 1. Buffer a(10)
  #1 ctor (10 bytes)
  #2 copy ctor from #1 (10 bytes)
== 2. Buffer b = a  -> copy ctor
  #3 copy ctor from #1 (10 bytes)
  ok: b.data() != a.data()  (own memory)
  ok: same content
  ok: changing b does not change a
== 3. Buffer c(5); c = a  -> copy assignment
  #4 ctor (5 bytes)
  #4 copy assign from #1 (10 bytes)
  ok: c is now a copy of a
== 4. Buffer d = std::move(a)  -> move ctor
  #5 move ctor from #1 (10 bytes)
  ok: a is empty (valid moved-from state)
  ok: d took a's memory, no copy
  ok: d has the original content
== 5. Buffer e(3); e = std::move(b)  -> move assignment
  #6 ctor (3 bytes)
  #6 move assign from #3 (10 bytes)
  ok: e took b's memory
== 6. e = e  -> self-assignment
  #6 copy assign from #6 (10 bytes)
  ok: e survived self-assignment
== 7. moved-from object is reusable
  #1 copy assign from #5 (10 bytes)
  ok: a = d after a was moved from
== end of main: destructors in reverse order
  #6 dtor (10 bytes)
  #5 dtor (10 bytes)
  #4 dtor (10 bytes)
  #3 dtor (0 bytes, moved-from)
  #2 dtor (10 bytes)
  #1 dtor (10 bytes)
*/
