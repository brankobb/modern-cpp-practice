// Korak 2 -- REŠENJE zadatka "Buffer: Rule of 5 od nule".
//   ./build.sh roadmap/step-2-object-lifecycle/solutions/buffer.cpp
//   ./build.sh roadmap/step-2-object-lifecycle/solutions/buffer.cpp -DNAIVE_ASSIGN
//       -- copy dodela koja PRVO briše pa kopira. Kod e = e su this i &other
//          isti objekat: posle delete[] i new[], other.data_ JE nova,
//          neinicijalizovana memorija, pa se smeće kopira samo u sebe i
//          sadržaj je izgubljen -- test 6 ispiše FAIL. ASan ništa ne prijavi:
//          nema pristupa oslobođenoj memoriji, samo tihi pogrešan rezultat.
//
// Svaki objekat dobija svoj broj (#1, #2, ...) pri konstrukciji; broj se NE
// kopira i ne pomera, pa se iz ispisa vidi tačno koji objekat šta radi.

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <utility>

class Buffer {
public:
    explicit Buffer(std::size_t size, std::uint8_t fill = 0)
        : id_{++next_id_}, size_{size}, data_{new std::uint8_t[size]} {
        std::fill_n(data_, size_, fill);
        std::printf("  #%d ctor (%zu bytes)\n", id_, size_);
    }

    ~Buffer() {
        std::printf("  #%d dtor (%zu bytes%s)\n", id_, size_, data_ == nullptr ? ", moved-from" : "");
        delete[] data_;                      // delete[] nullptr je dozvoljen i ne radi ništa
    }

    // Copy ctor: NOVI objekat, sopstvena memorija, isti sadržaj (deep copy).
    Buffer(const Buffer& other) : id_{++next_id_}, size_{other.size_}, data_{new std::uint8_t[other.size_]} {
        std::copy_n(other.data_, size_, data_);
        std::printf("  #%d copy ctor from #%d (%zu bytes)\n", id_, other.id_, size_);
    }

    // Copy dodela: POSTOJEĆI objekat već drži memoriju.
    Buffer& operator=(const Buffer& other) {
        std::printf("  #%d copy assign from #%d (%zu bytes)\n", id_, other.id_, other.size_);
#ifdef NAIVE_ASSIGN
        delete[] data_;                      // za e = e: upravo smo obrisali i sadržaj izvora
        data_ = new std::uint8_t[other.size_];
        std::copy_n(other.data_, other.size_, data_);   // other.data_ == data_: kopira smeće u sebe
        size_ = other.size_;
#else
        // 1) nova memorija i kopija, 2) tek onda brisanje stare.
        // Self-assignment je bezbedan BEZ posebne provere (kopiramo pre brisanja),
        // a ako new baci, *this ostaje netaknut (strong garancija).
        std::uint8_t* fresh = new std::uint8_t[other.size_];
        std::copy_n(other.data_, other.size_, fresh);
        delete[] data_;
        data_ = fresh;
        size_ = other.size_;
#endif
        return *this;                        // omogućava a = b = c
    }

    // Move ctor: preuzmi pokazivač, izvor ostavi PRAZAN ali ISPRAVAN
    // (destruktor i dodela nad njim moraju da rade).
    Buffer(Buffer&& other) noexcept
        : id_{++next_id_}, size_{std::exchange(other.size_, 0)}, data_{std::exchange(other.data_, nullptr)} {
        std::printf("  #%d move ctor from #%d (%zu bytes)\n", id_, other.id_, size_);
    }

    // Move dodela: oslobodi svoje, preuzmi tuđe, izvor isprazni.
    Buffer& operator=(Buffer&& other) noexcept {
        std::printf("  #%d move assign from #%d (%zu bytes)\n", id_, other.id_, other.size_);
        if (this != &other) {                // a = std::move(a): bez provere bismo obrisali sopstvenu memoriju
            delete[] data_;
            data_ = std::exchange(other.data_, nullptr);
            size_ = std::exchange(other.size_, 0);
        }
        return *this;
    }

    std::size_t size() const noexcept { return size_; }
    std::uint8_t* data() noexcept { return data_; }
    const std::uint8_t* data() const noexcept { return data_; }   // const overload: const Buffer& vraća const podatke
    int id() const noexcept { return id_; }

private:
    int id_;                                 // redosled deklaracije = redosled inicijalizacije
    std::size_t size_;
    std::uint8_t* data_;
    static inline int next_id_ = 0;
};

bool same_content(const Buffer& a, const Buffer& b) {
    return a.size() == b.size() && std::equal(a.data(), a.data() + a.size(), b.data());
}

void check(bool ok, const char* what) { std::printf("  %s: %s\n", ok ? "ok" : "FAIL", what); }

int main() {
    std::printf("== 1. Buffer a(10)\n");
    Buffer a(10, 0xAB);
    a.data()[0] = 1;
    const Buffer original = a;               // za kasnija poređenja (i ovo je copy ctor)

    std::printf("== 2. Buffer b = a  -> copy ctor\n");
    Buffer b = a;
    check(b.data() != a.data(), "b.data() != a.data()  (own memory)");
    check(same_content(a, b), "same content");
    b.data()[1] = 7;
    check(a.data()[1] == 0xAB, "changing b does not change a");

    std::printf("== 3. Buffer c(5); c = a  -> copy assignment\n");
    Buffer c(5);
    c = a;
    check(c.size() == 10 && same_content(c, a), "c is now a copy of a");

    std::printf("== 4. Buffer d = std::move(a)  -> move ctor\n");
    const std::uint8_t* a_memory = a.data();
    Buffer d = std::move(a);
    check(a.data() == nullptr && a.size() == 0, "a is empty (valid moved-from state)");
    check(d.data() == a_memory, "d took a's memory, no copy");
    check(same_content(d, original), "d has the original content");

    std::printf("== 5. Buffer e(3); e = std::move(b)  -> move assignment\n");
    Buffer e(3);
    e = std::move(b);
    check(b.data() == nullptr && e.size() == 10 && e.data()[1] == 7, "e took b's memory");

    std::printf("== 6. e = e  -> self-assignment\n");
    Buffer& alias = e;                       // realan slučaj: v[i] = v[j] kad je i == j
    e = alias;
    check(e.size() == 10 && e.data()[1] == 7, "e survived self-assignment");

    std::printf("== 7. moved-from object is reusable\n");
    a = d;                                   // dodela u moved-from objekat mora da radi
    check(same_content(a, d), "a = d after a was moved from");

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
