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

    // Ako "ukradeš" pokazivač u move ctor-u ali NE postaviš other.data_ =
    // nullptr (NIJE DOBRO) jer oba objekta (this i other) onda "misle" da
    // poseduju ISTI pokazivač -- kad other izađe iz scope-a i pozove se
    // ~Resource(), oslobodiće memoriju koju TVOJ objekat i dalje koristi
    // (use-after-free čim je ti pokušaš da koristiš, double-free kad se i
    // tvoj destruktor pozove).
    // Treba da UVEK ostaviš other u "praznom ali validnom" stanju
    // (nullptr / 0) posle krađe -- da njegov destruktor bude bezopasan
    // no-op.
    // Možeš i koristiti std::exchange(other.data_, nullptr) umesto
    // ručnog "ukradi pa nuluj" -- kraće, isti efekat, manje šanse da
    // zaboraviš korak.
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
    std::cout << "-- sa std::move (treba: move ctor) --\n";
    Resource a(10);
    Resource b = std::move(a); // treba: move ctor
    (void)b;

    std::cout << "-- bez std::move (treba: copy ctor) --\n";
    Resource c(10);
    Resource d = c; // BEZ std::move -- c je lvalue, poziva se copy ctor
    (void)d;
}
