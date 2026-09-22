// EXPECT-UB: attempting double-free
// POGREŠNO: "= default" move konstruktor u klasi koja poseduje sirov pokazivač.
// Zašto: kompajlerov move pomera član po član, a "move" za int* je KOPIJA
//   vrednosti pokazivača; izvor ostaje nepromenjen. Posle Buffer b =
//   std::move(a) oba pokazuju na isti niz, pa ga oba destruktora obrišu.
//   = default je ispravan samo kad svaki član sam ispravno radi move.
// Ispravno: ručni move sa std::exchange(other.data_, nullptr) (week1 s04),
//   ili član std::unique_ptr<int[]> -- tada je = default (ili rule of 0) tačan.
#include <cstdio>
#include <utility>

class Buffer {
public:
    explicit Buffer(int n) : data_(new int[n]{}), size_(n) {}
    Buffer(Buffer&&) = default;
    ~Buffer() { delete[] data_; }
    int size() const { return size_; }

private:
    int* data_;
    int size_;
};

int main() {
    Buffer a(4);
    Buffer b = std::move(a);
    std::printf("%d\n", b.size());
}
