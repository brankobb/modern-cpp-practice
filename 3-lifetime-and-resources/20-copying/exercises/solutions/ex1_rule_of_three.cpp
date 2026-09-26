// Rešenje zadatka ex1_rule_of_three.

#include <cstddef>
#include <cstring>
#include <iostream>
#include <utility>

// Rule of 3: klasa koja sama upravlja resursom (ovde: heap niz) mora sama
// da napiše destruktor, copy konstruktor i copy dodelu -- ono što kompajler
// generiše kopira POKAZIVAČ (plitka kopija, pa double free: ub/ ove lekcije).
class Text {
public:
    // Korak 1
    explicit Text(const char* s) : length_(std::strlen(s)), data_(new char[length_ + 1]) {
        std::memcpy(data_, s, length_ + 1);
    }
    ~Text() { delete[] data_; }

    const char* c_str() const { return data_; }
    std::size_t length() const { return length_; }
    void set(std::size_t i, char c) { data_[i] = c; }

    // Korak 2: duboka kopija -- sopstveni blok.
    Text(const Text& o) : length_(o.length_), data_(new char[o.length_ + 1]) {
        std::memcpy(data_, o.data_, length_ + 1);
    }

    // Korak 3: copy-and-swap. Sva "opasna" posla (alokacija) su u kopiji;
    // swap ne baca. Dodela samom sebi napravi kopiju sebe -- nepotrebno,
    // ali ispravno.
    Text& operator=(const Text& o) {
        Text copy(o);
        swap(copy);
        return *this;
    }   // ovde se copy (sa STARIM podacima) uništi

    void swap(Text& o) noexcept {
        std::swap(length_, o.length_);
        std::swap(data_, o.data_);
    }

private:
    std::size_t length_;   // deklarisan prvi: data_ koristi length_ u init listi
    char* data_;
};

int main() {
    Text a("motor");
    Text b(a);
    b.set(0, 'M');
    std::cout << "a: " << a.c_str() << ", b: " << b.c_str() << ", length " << b.length() << '\n';

    Text c("x");
    c = a;
    a.set(4, 'R');
    std::cout << "a: " << a.c_str() << ", c: " << c.c_str() << '\n';
    Text& same = c;
    c = same;
    std::cout << "after c = c: " << c.c_str() << '\n';
    Text d("d"), e("e");
    d = e = b;
    std::cout << "d: " << d.c_str() << ", e: " << e.c_str() << '\n';
}
