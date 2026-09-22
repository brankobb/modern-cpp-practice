// STD: c++17
// EXPECT-GCC: ambiguous overload for 'operator='
// EXPECT-CLANG: use of overloaded operator '=' is ambiguous
// POGREŠNO: i operator=(Buffer) (po vrednosti) i operator=(Buffer&&).
// Zašto: za a = std::move(b) oba su jednako dobra: parametar po vrednosti
//   prima rvalue (move konstruktorom), a Buffer&& ga prima direktno.
//   Rangiranje konverzija ne daje prednost nijednom, pa je poziv dvosmislen.
// Ispravno: izaberi jedno. Ili JEDAN operator=(Buffer other) za obe dodele
//   (main.cpp, sekcija 5), ili par operator=(const Buffer&) +
//   operator=(Buffer&&) (sekcija 1).
#include <utility>

class Buffer {
public:
    Buffer() = default;
    Buffer(const Buffer&) = default;
    Buffer(Buffer&&) noexcept = default;
    Buffer& operator=(Buffer other) noexcept {
        std::swap(value_, other.value_);
        return *this;
    }
    Buffer& operator=(Buffer&& other) noexcept {
        value_ = other.value_;
        return *this;
    }

private:
    int value_ = 0;
};

int main() {
    Buffer a;
    Buffer b;
    a = std::move(b);
}
