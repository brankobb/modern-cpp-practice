#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

// Lekcija 25 -- rešenje vežbe Buffer: rule of 3 -> rule of 5 -> rule of 0.
// Vežba (bez rešenja) je u exercise.cpp. Sve se kompajlira i radi bez
// ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 3-zivotni-vek-i-resursi/25-vezba-bafer  proverava oba.

struct Counters {
    int copies = 0;
    int moves = 0;
};

// ---------------------------------------------------------------- 1
// Rule of 5: klasa ručno poseduje memoriju, pa piše svih pet funkcija.
class Buffer {
public:
    explicit Buffer(std::size_t size) : data_(new int[size]{}), size_(size) {}

    ~Buffer() { delete[] data_; }                                         // 1

    Buffer(const Buffer& other) : data_(new int[other.size_]), size_(other.size_) { // 2
        std::copy(other.data_, other.data_ + size_, data_);
        ++counters.copies;
    }

    Buffer(Buffer&& other) noexcept                                      // 3
        : data_(std::exchange(other.data_, nullptr)), size_(std::exchange(other.size_, 0)) {
        ++counters.moves;
    }

    Buffer& operator=(const Buffer& other) {                             // 4: copy-and-swap
        Buffer copy(other);
        swap(*this, copy);
        return *this;
    }

    Buffer& operator=(Buffer&& other) noexcept {                         // 5
        if (this != &other) {
            delete[] data_;
            data_ = std::exchange(other.data_, nullptr);
            size_ = std::exchange(other.size_, 0);
            ++counters.moves;
        }
        return *this;
    }

    // swap menja ČLANOVE. std::swap(*this, other) bi zvao move dodelu, koja
    // zove swap... beskonačna rekurzija (ub/u02).
    friend void swap(Buffer& a, Buffer& b) noexcept {
        std::swap(a.data_, b.data_);
        std::swap(a.size_, b.size_);
    }

    std::size_t size() const { return size_; }
    int& operator[](std::size_t i) { return data_[i]; }
    const int& operator[](std::size_t i) const { return data_[i]; }

    inline static Counters counters;

private:
    int* data_;
    std::size_t size_;
};

void s01_ruleOfFive() {
    std::cout << "-- 1. rule of 5 --\n";
    Buffer::counters = {};
    Buffer a(10);
    a[0] = 42;
    Buffer b = a;            // copy ctor
    b[0] = 7;
    Buffer c = std::move(b); // move ctor; b postaje prazan
    Buffer d(1);
    d = a;                   // copy dodela (kopija u copy-and-swap)
    d = std::move(c);        // move dodela
    Buffer& alias = a;
    a = alias;               // copy dodela samom sebi
    a = std::move(alias);    // move dodela samom sebi: provera this != &other
    std::cout << "  a[0]=" << a[0] << " d[0]=" << d[0] << " b.size()=" << b.size() << " (moved-from)"
              << "\n  kopija=" << Buffer::counters.copies << " move=" << Buffer::counters.moves << "\n";
}

// ---------------------------------------------------------------- 2
Buffer makeFilled(std::size_t n, int value) {
    Buffer result(n);
    for (std::size_t i = 0; i < n; ++i) result[i] = value;
    return result; // NRVO: kompajler obično pravi result direktno u pozivaocu (nije garantovano, lekcija 24)
}

void s02_returnByValue() {
    std::cout << "-- 2. vraćanje po vrednosti --\n";
    Buffer::counters = {};
    Buffer filled = makeFilled(5, 3);
    std::cout << "  makeFilled(5, 3): filled[4]=" << filled[4] << ", kopija=" << Buffer::counters.copies
              << " move=" << Buffer::counters.moves << "  <- ni kopija ni move (NRVO)\n";
}

// ---------------------------------------------------------------- 3
// Isti Buffer, ali move konstruktor BEZ noexcept.
class SlowBuffer {
public:
    explicit SlowBuffer(std::size_t size) : data_(new int[size]{}), size_(size) {}
    ~SlowBuffer() { delete[] data_; }
    SlowBuffer(const SlowBuffer& other) : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
        ++counters.copies;
    }
    SlowBuffer(SlowBuffer&& other) // nema noexcept!
        : data_(std::exchange(other.data_, nullptr)), size_(std::exchange(other.size_, 0)) {
        ++counters.moves;
    }
    SlowBuffer& operator=(const SlowBuffer&) = delete;
    SlowBuffer& operator=(SlowBuffer&&) = delete;

    inline static Counters counters;

private:
    int* data_;
    std::size_t size_;
};

void s03_vectorReallocation() {
    std::cout << "-- 3. std::vector realokacija: noexcept move --\n";
    Buffer::counters = {};
    SlowBuffer::counters = {};
    std::vector<Buffer> fast;
    std::vector<SlowBuffer> slow;
    for (int i = 0; i < 5; ++i) {
        fast.push_back(Buffer(4)); // bez reserve: kapacitet raste 1, 2, 4, 8 -> tri realokacije
        slow.push_back(SlowBuffer(4));
    }
    std::cout << "  5 x push_back, Buffer (noexcept move):  kopija=" << Buffer::counters.copies
              << " move=" << Buffer::counters.moves << "\n";
    std::cout << "  5 x push_back, SlowBuffer (move bez noexcept): kopija=" << SlowBuffer::counters.copies
              << " move=" << SlowBuffer::counters.moves << "  <- realokacija KOPIRA (strong garancija, lekcija 23)\n";
}

// ---------------------------------------------------------------- 4
// Rule of 0: isto ponašanje, nula specijalnih funkcija. Članovi sami znaju
// da se kopiraju, pomeraju i unište.
struct Buffer0 {
    explicit Buffer0(std::size_t size) : data(size, 0) {}
    std::vector<int> data;
};

// Rule of 0 za tip koji se samo pomera: unique_ptr zabrani kopiju sam.
struct UniqueBuffer {
    explicit UniqueBuffer(std::size_t n) : data(std::make_unique<int[]>(n)), size(n) {}
    std::unique_ptr<int[]> data;
    std::size_t size;
};

void s04_ruleOfZero() {
    std::cout << "-- 4. rule of 0 --\n";
    Buffer0 a(10);
    a.data[0] = 42;
    Buffer0 b = a;
    b.data[0] = 7;
    Buffer0 c = std::move(b);
    Buffer0& alias = a;
    a = alias; // i dodela samom sebi radi
    std::cout << std::boolalpha << "  Buffer0: a[0]=" << a.data[0] << " c[0]=" << c.data[0]
              << ", nothrow move=" << std::is_nothrow_move_constructible_v<Buffer0>
              << ", kopija=" << std::is_copy_constructible_v<Buffer0> << "\n";
    std::cout << "  UniqueBuffer: nothrow move=" << std::is_nothrow_move_constructible_v<UniqueBuffer>
              << ", kopija=" << std::is_copy_constructible_v<UniqueBuffer> << "  <- move-only bez ijedne linije\n"
              << std::noboolalpha;
}

// ---------------------------------------------------------------- 5
// Varijanta: JEDAN operator= po vrednosti pokriva i copy i move dodelu
// (copy-and-swap). Kraće, strong garancija, cena je jedan move više.
class Buffer4 {
public:
    explicit Buffer4(std::size_t size) : data_(new int[size]{}), size_(size) {}
    ~Buffer4() { delete[] data_; }
    Buffer4(const Buffer4& other) : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
        ++counters.copies;
    }
    Buffer4(Buffer4&& other) noexcept
        : data_(std::exchange(other.data_, nullptr)), size_(std::exchange(other.size_, 0)) {
        ++counters.moves;
    }
    Buffer4& operator=(Buffer4 other) noexcept { // parametar: copy ctor za lvalue, move ctor za rvalue
        swap(*this, other);
        return *this;
    }
    friend void swap(Buffer4& a, Buffer4& b) noexcept {
        std::swap(a.data_, b.data_);
        std::swap(a.size_, b.size_);
    }
    std::size_t size() const { return size_; }

    inline static Counters counters;

private:
    int* data_;
    std::size_t size_;
};

void s05_unifiedAssignment() {
    std::cout << "-- 5. jedan operator= po vrednosti (copy-and-swap za oba) --\n";
    Buffer4 a(3);
    Buffer4 b(5);
    Buffer4::counters = {};
    a = b;            // kopija u parametar, pa swap
    int copiesForCopy = Buffer4::counters.copies;
    Buffer4::counters = {};
    a = std::move(b); // move u parametar, pa swap
    std::cout << "  a = b: kopija=" << copiesForCopy << "; a = std::move(b): kopija=" << Buffer4::counters.copies
              << " move=" << Buffer4::counters.moves << " (namenski move = bi bio 0 move ctor-a)\n";
    // Ne piši i operator=(Buffer4) i operator=(Buffer4&&): dvosmisleno (errors/e01).
}

int main() {
    s01_ruleOfFive();
    s02_returnByValue();
    s03_vectorReallocation();
    s04_ruleOfZero();
    s05_unifiedAssignment();
}
