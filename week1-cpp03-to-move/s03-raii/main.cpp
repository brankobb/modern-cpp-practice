#include <algorithm>
#include <iostream>
#include <stdexcept>

// Vežba: implementiraj copy-and-swap za klasu koja poseduje resurs.
//   1) operator= prima parametar PO VREDNOSTI (poziva copy ctor)
//   2) swap(*this, other)
//   3) other (lokalna kopija) se uništi na kraju funkcije, noseći stari resurs
//
// Zatim demonstriraj strong exception safety: napravi scenario gde bi
// "naivan" operator= ostavio objekat u polu-validnom stanju da je throw-ovao
// usred kopiranja, a copy-and-swap verzija ostane nepromenjena.

class Resource {
public:
    explicit Resource(int size) : data_(new int[size]), size_(size) {}
    Resource(const Resource& other) : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
    }
    ~Resource() { delete[] data_; }

    friend void swap(Resource& a, Resource& b) noexcept {
        std::swap(a.data_, b.data_);
        std::swap(a.size_, b.size_);
    }

    Resource& operator=(Resource other) { // po vrednosti -> copy-and-swap
        swap(*this, other);
        return *this;
    }

private:
    int* data_;
    int size_;
};

int main() {
    Resource a(10);
    Resource b(20);
    a = b; // testiraj copy-and-swap
}
