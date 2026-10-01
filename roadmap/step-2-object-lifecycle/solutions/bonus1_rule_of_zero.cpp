// Korak 2 -- BONUS 1: isti Buffer bez golog new[]/delete[].
//   ./build.sh roadmap/step-2-object-lifecycle/solutions/bonus1_rule_of_zero.cpp
//
// Dve verzije, jer "unique_ptr umesto new[]" NIJE odmah Rule of 0:
//
//   BufferU -- std::unique_ptr<std::uint8_t[]>
//     destruktor i move dobijaš besplatno; COPY je automatski obrisan
//     (unique_ptr se ne kopira). Ako ti treba deep copy, pišeš copy ctor i
//     copy dodelu -- a čim napišeš copy, kompajler PRESTAJE da generiše
//     move, pa move moraš da vratiš sa "= default". Dakle: 4 deklaracije,
//     ali nijedan delete[], nijedna provera self-move-a, nijedno curenje
//     ako new baci.
//
//   BufferV -- std::vector<std::uint8_t>
//     pravi Rule of 0: ne pišeš NIŠTA od specijalnih funkcija, a dobijaš
//     ispravan deep copy, move, destruktor i self-assignment. Član koji
//     sam upravlja resursom => klasa ne mora.
//
// Pravilo za praksu: resurs drži JEDNA mala klasa (unique_ptr, vector,
// tvoj File iz Koraka 1); sve ostale klase samo slažu takve članove i
// ne pišu ni destruktor ni kopiju (C.20).

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

class BufferU {
public:
    explicit BufferU(std::size_t size, std::uint8_t fill = 0)
        : size_{size}, data_{std::make_unique<std::uint8_t[]>(size)} {
        std::fill_n(data_.get(), size_, fill);
    }

    // Deep copy moraš sam: unique_ptr ne zna kako da kopira niz.
    BufferU(const BufferU& other) : size_{other.size_}, data_{std::make_unique<std::uint8_t[]>(other.size_)} {
        std::copy_n(other.data_.get(), size_, data_.get());
    }
    BufferU& operator=(const BufferU& other) {
        BufferU tmp{other};                  // copy-and-swap: kopija pa zamena
        swap(tmp);                           // self-assignment i strong garancija dolaze sami
        return *this;
    }
    // Napisan copy => move se NE generiše. Vrati ga:
    BufferU(BufferU&& other) noexcept : size_{std::exchange(other.size_, 0)}, data_{std::move(other.data_)} {}
    BufferU& operator=(BufferU&& other) noexcept {
        BufferU tmp{std::move(other)};       // izvor je sad prazan, a self-move je bezbedan
        swap(tmp);
        return *this;
    }
    // Destruktor: NE pišeš. ~unique_ptr radi delete[].

    void swap(BufferU& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(data_, other.data_);
    }

    std::size_t size() const noexcept { return size_; }
    std::uint8_t* data() noexcept { return data_.get(); }
    const std::uint8_t* data() const noexcept { return data_.get(); }

private:
    std::size_t size_;
    std::unique_ptr<std::uint8_t[]> data_;
};

// Napomena: "= default" za move ne bi bio dovoljan -- default move pomera
// unique_ptr (izvor postaje nullptr), ali size_ je obična vrednost i KOPIRA
// se, pa bi moved-from objekat imao size() == 10 i data() == nullptr:
// pokvarena invarijanta. Zato su move funkcije gore napisane ručno.

class BufferV {
public:
    explicit BufferV(std::size_t size, std::uint8_t fill = 0) : data_(size, fill) {}
    // (zagrade, ne {}: vector{10, 0xAB} bi bio vector od DVA elementa -- initializer_list ima prednost)

    std::size_t size() const noexcept { return data_.size(); }
    std::uint8_t* data() noexcept { return data_.data(); }
    const std::uint8_t* data() const noexcept { return data_.data(); }

private:
    std::vector<std::uint8_t> data_;         // i to je sve
};

// Šta je kompajler generisao -- provereno pri kompajliranju:
static_assert(std::is_copy_constructible_v<BufferU> && std::is_nothrow_move_constructible_v<BufferU>);
static_assert(std::is_copy_constructible_v<BufferV> && std::is_nothrow_move_constructible_v<BufferV>);

struct OnlyUniquePtr {                       // unique_ptr bez ičega napisanog:
    std::unique_ptr<std::uint8_t[]> p;
};
static_assert(!std::is_copy_constructible_v<OnlyUniquePtr>, "copy je obrisan");
static_assert(std::is_nothrow_move_constructible_v<OnlyUniquePtr>, "move je besplatan");

template <typename B>
void exercise(const char* name) {
    std::printf("== %s\n", name);
    B a(10, 0xAB);
    B b = a;
    std::printf("  copy: own memory %s, same content %s\n", b.data() != a.data() ? "yes" : "no",
                std::equal(a.data(), a.data() + a.size(), b.data()) ? "yes" : "no");
    B c(3);
    c = a;
    B& alias = c;
    c = alias;
    std::printf("  copy assign + self-assign: size %zu, c[0] = 0x%X\n", c.size(), c.data()[0]);
    const std::uint8_t* mem = a.data();
    B d = std::move(a);
    std::printf("  move: memory taken %s, source size %zu\n", d.data() == mem ? "yes" : "no", a.size());
    a = d;
    std::printf("  moved-from reusable: size %zu\n", a.size());
}

int main() {
    exercise<BufferU>("BufferU (unique_ptr: copy written, move restored)");
    exercise<BufferV>("BufferV (vector: Rule of 0)");
}

/* EXPECTED OUTPUT
== BufferU (unique_ptr: copy written, move restored)
  copy: own memory yes, same content yes
  copy assign + self-assign: size 10, c[0] = 0xAB
  move: memory taken yes, source size 0
  moved-from reusable: size 10
== BufferV (vector: Rule of 0)
  copy: own memory yes, same content yes
  copy assign + self-assign: size 10, c[0] = 0xAB
  move: memory taken yes, source size 0
  moved-from reusable: size 10
*/
