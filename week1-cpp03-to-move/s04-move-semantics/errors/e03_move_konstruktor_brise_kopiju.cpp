// STD: c++17
// EXPECT-GCC: use of deleted function 'constexpr Buffer::Buffer(const Buffer&)'
// EXPECT-CLANG: call to implicitly-deleted copy constructor of 'Buffer'
// POGREŠNO: klasa ima korisnički move konstruktor, a kod je kopira.
// Zašto: čim klasa deklariše move konstruktor ili move dodelu, kompajler
//   OBRIŠE copy konstruktor i copy dodelu ([class.copy.ctor]). Logika: ako
//   si morao sam da napišeš move, kompajlerova kopija član po član verovatno
//   nije ispravna. Ceo skup pravila: week2 s06.
// Ispravno: napiši i copy konstruktor (rule of 5, s05), ili ostavi klasu
//   move-only i prenosi je sa std::move.
#include <utility>

class Buffer {
public:
    Buffer() = default;
    Buffer(Buffer&& other) noexcept : data_(std::exchange(other.data_, nullptr)) {}
    ~Buffer() { delete[] data_; }

private:
    int* data_ = nullptr;
};

int main() {
    Buffer a;
    Buffer b = a;
    (void)b;
}
