#include <cstring>
#include <iostream>

// Vežba: klasa koja poseduje resurs (npr. char* na heap-u) sa NAIVNIM
// kompajlerski-generisanim kopiranjem (ne piši copy ctor/assignment).
// Napravi dva objekta, kopiraj jedan u drugi, izmeni jedan i pokaži da je
// drugi takođe promenjen (shallow copy bug). Pokreni pod ASan-om — očekuj
// double-free ili use-after-free kad oba destruktora oslobode isti pokazivač.
//
// Zatim popravi: napiši copy ctor i copy assignment koji rade DEEP copy,
// uključujući self-assignment proveru u operator=.

class NaiveString {
public:
    explicit NaiveString(const char* s) {
        data_ = new char[std::strlen(s) + 1];
        std::strcpy(data_, s);
    }
    ~NaiveString() { delete[] data_; }
    // Namerno bez copy ctor / copy assignment -- posmatraj šta se dešava.

private:
    char* data_;
};

int main() {
    NaiveString a("hello");
    NaiveString b = a; // shallow copy
    (void)b;
}
