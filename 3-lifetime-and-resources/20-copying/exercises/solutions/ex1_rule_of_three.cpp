// Rešenje zadatka ex1_rule_of_three.

#include <cstddef>
#include <cstring>
#include <iostream>
#include <utility>

// Rule of 3: klasa koja sama upravlja resursom (ovde: heap niz) mora sama
// da napiše destruktor, copy konstruktor i copy dodelu -- ono što kompajler
// generiše kopira POKAZIVAČ (plitka kopija, pa double free: ub/ ove lekcije).
class Tekst {
public:
    // Korak 1
    explicit Tekst(const char* s) : duzina_(std::strlen(s)), podaci_(new char[duzina_ + 1]) {
        std::memcpy(podaci_, s, duzina_ + 1);
    }
    ~Tekst() { delete[] podaci_; }

    const char* c_str() const { return podaci_; }
    std::size_t duzina() const { return duzina_; }
    void promeni(std::size_t i, char c) { podaci_[i] = c; }

    // Korak 2: duboka kopija -- sopstveni blok.
    Tekst(const Tekst& o) : duzina_(o.duzina_), podaci_(new char[o.duzina_ + 1]) {
        std::memcpy(podaci_, o.podaci_, duzina_ + 1);
    }

    // Korak 3: copy-and-swap. Sva "opasna" posla (alokacija) su u kopiji;
    // swap ne baca. Dodela samom sebi napravi kopiju sebe -- nepotrebno,
    // ali ispravno.
    Tekst& operator=(const Tekst& o) {
        Tekst kopija(o);
        swap(kopija);
        return *this;
    }   // ovde se kopija (sa STARIM podacima) uništi

    void swap(Tekst& o) noexcept {
        std::swap(duzina_, o.duzina_);
        std::swap(podaci_, o.podaci_);
    }

private:
    std::size_t duzina_;   // deklarisan prvi: podaci_ koristi duzina_ u init listi
    char* podaci_;
};

int main() {
    Tekst a("motor");
    Tekst b(a);
    b.promeni(0, 'M');
    std::cout << "a: " << a.c_str() << ", b: " << b.c_str() << ", dužina " << b.duzina() << '\n';

    Tekst c("x");
    c = a;
    a.promeni(4, 'R');
    std::cout << "a: " << a.c_str() << ", c: " << c.c_str() << '\n';
    Tekst& isti = c;
    c = isti;
    std::cout << "posle c = c: " << c.c_str() << '\n';
    Tekst d("d"), e("e");
    d = e = b;
    std::cout << "d: " << d.c_str() << ", e: " << e.c_str() << '\n';
}
