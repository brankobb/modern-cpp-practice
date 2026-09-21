#include <algorithm>
#include <iostream>
#include <utility>

// TODO korak 1: implementiraj rule of 3 (ctor, dtor, copy ctor, copy
// assignment preko copy-and-swap). Ostavi std::cout log u svakom da vidiš
// šta se poziva.
//
// TODO korak 2: dodaj move ctor i move assignment (noexcept), pa proveri
// ponašanje u std::vector<Buffer> (push_back izaziva realokaciju -- move
// se koristi samo ako je noexcept ili nema copy ctor-a).

class Buffer {
public:
    explicit Buffer(std::size_t size) : data_(new int[size]), size_(size) {
        std::cout << "ctor(" << size_ << ")\n";
    }

    ~Buffer() {
        std::cout << "dtor(" << size_ << ")\n";
        delete[] data_;
    }

    // TODO: copy ctor
    // TODO: friend void swap(Buffer&, Buffer&) noexcept
    // TODO: operator=(Buffer other) -- copy-and-swap
    // TODO: move ctor(Buffer&&) noexcept
    // TODO: move assignment(Buffer&&) noexcept

    std::size_t size() const { return size_; }

private:
    int* data_;
    std::size_t size_;
};

int main() {
    Buffer a(10);
    // Buffer b = a;             // treba copy ctor
    // Buffer c = std::move(a);  // treba move ctor
}
