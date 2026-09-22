// STD: c++17
// EXPECT-GCC: no matching function for call to 'Base::Base()'
// EXPECT-CLANG: must explicitly initialize the base class 'Base' which does not have a default constructor
// POGREŠNO: konstruktor izvedene klase ne poziva konstruktor baze, a baza
//   nema podrazumevani.
// Zašto: bazni deo se pravi PRE tela izvedenog konstruktora. Ako ga init
//   lista ne navede, pokušava se Base(), a on ne postoji.
// Ispravno: Derived() : Base(42) {} -- baza ide u init listu, pre članova.
class Base {
public:
    explicit Base(int id) : id_(id) {}
    virtual ~Base() = default;
    int id() const { return id_; }

private:
    int id_;
};

class Derived : public Base {
public:
    Derived() {}
};

int main() {
    Derived d;
    return d.id();
}
