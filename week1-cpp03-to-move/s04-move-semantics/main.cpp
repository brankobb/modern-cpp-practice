#include <iostream>
#include <utility>

// Vežba: dodaj move ctor i move assignment klasi iz s03 (Resource).
//   Resource(Resource&& other) noexcept;
//   Resource& operator=(Resource&& other) noexcept;
// Move treba da "ukrade" pokazivač i ostavi other u validnom praznom stanju
// (npr. data_ = nullptr, size_ = 0), tako da other-ov destruktor bude no-op.
//
// Testiraj:
//  - Resource a(10); Resource b = std::move(a); -> pozvan move ctor?
//  - dodaj std::cout u svaki ctor/assignment da vidiš koji se poziva
//  - probaj bez std::move (treba da se pozove copy) i sa std::move
//    (treba da se pozove move)

class Resource {
public:
    explicit Resource(int size) : data_(new int[size]), size_(size) {
        std::cout << "ctor(size)\n";
    }
    Resource(const Resource&) { std::cout << "copy ctor\n"; }
    Resource(Resource&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        std::cout << "move ctor\n";
        other.data_ = nullptr;
        other.size_ = 0;
    }
    ~Resource() { delete[] data_; }

private:
    int* data_ = nullptr;
    int size_ = 0;
};

int main() {
    Resource a(10);
    Resource b = std::move(a); // treba: move ctor
    (void)b;
}
