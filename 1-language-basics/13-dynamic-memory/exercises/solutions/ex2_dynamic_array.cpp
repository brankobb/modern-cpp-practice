// Rešenje zadatka ex2_dynamic_array.

#include <cstddef>
#include <iostream>
#include <stdexcept>

class DynamicArray {
public:
    DynamicArray() = default;
    ~DynamicArray() { delete[] data_; }   // delete[] nullptr je dozvoljen (ne radi ništa)

    // Korak 1: vlasnik bloka se ne sme plitko kopirati. Pravu kopiju
    // (novi blok + prepisivanje) radi lekcija 20; ovde je zabranjena.
    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;

    // Korak 2: udvostručavanje daje amortizovano O(1) po dodavanju.
    // Novi blok se pravi PRE brisanja starog: ako new baci bad_alloc,
    // objekat ostaje nepromenjen (strong guarantee).
    void add(int x) {
        if (size_ == cap_) {
            std::size_t newCap = cap_ ? 2 * cap_ : 1;
            int* block = new int[newCap];
            for (std::size_t i = 0; i < size_; ++i) block[i] = data_[i];
            delete[] data_;
            data_ = block;
            std::cout << "grow: " << cap_ << " -> " << newCap << '\n';
            cap_ = newCap;
        }
        data_[size_++] = x;
    }

    // Korak 3
    int at(std::size_t i) const {
        if (i >= size_) throw std::out_of_range("DynamicArray::at");
        return data_[i];
    }
    std::size_t size() const { return size_; }
    std::size_t capacity() const { return cap_; }

private:
    int* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t cap_ = 0;
};

int main() {
    DynamicArray a;
    for (int i = 1; i <= 10; ++i) a.add(i);
    int sum = 0;
    for (std::size_t i = 0; i < a.size(); ++i) sum += a.at(i);
    std::cout << "size: " << a.size() << ", capacity: " << a.capacity()
              << ", sum: " << sum << '\n';
    try {
        a.at(10);
    } catch (const std::out_of_range&) {
        std::cout << "at(10): out_of_range\n";
    }
}
