// STD: c++17
// EXPECT-GCC: no matching function for call to 'Base::Base()'
// EXPECT-CLANG: must explicitly initialize the base class 'Base' which does not have a default constructor
// POGREŠNO: sopstveni copy konstruktor izvedene klase ne kopira baznu klasu.
// Zašto: ako init lista ne navede bazu, ona se pravi PODRAZUMEVANIM
//   konstruktorom, ne copy konstruktorom. Ovde Base nema podrazumevani,
//   pa je greška vidljiva. Kad Base IMA podrazumevani, kod se tiho
//   kompajlira i kopija ima pogrešan bazni deo (main.cpp, sekcija 5), a
//   ni g++ ni clang sa -Wall -Wextra ne upozore (EC++ Item 12).
// Ispravno: Derived(const Derived& other) : Base(other), extra_(other.extra_) {}
//   -- ili ga ne piši, kompajlerov kopira sve.
class Base {
public:
    explicit Base(int id) : id_(id) {}
    int id() const { return id_; }

private:
    int id_;
};

class Derived : public Base {
public:
    explicit Derived(int id) : Base(id) {}
    Derived(const Derived& other) : extra_(other.extra_) {}

private:
    int extra_ = 0;
};

int main() {
    Derived a(1);
    Derived b(a);
    return b.id();
}
