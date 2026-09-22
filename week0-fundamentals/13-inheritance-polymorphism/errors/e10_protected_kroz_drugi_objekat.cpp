// STD: c++17
// EXPECT-GCC: 'int Base::value_' is protected within this context
// EXPECT-CLANG: 'value_' is a protected member of 'Base'
// POGREŠNO: izvedena klasa čita protected član preko reference na BAZU.
// Zašto: protected pristup važi samo kroz objekte SVOG tipa (this, ili
//   const Derived&), ne kroz bilo koji Base ([class.protected]). Inače bi
//   svaka izvedena klasa mogla da čita protected podatke objekata SASVIM
//   DRUGIH izvedenih klasa, samo prosleđenih kao Base&.
// Ispravno: parametar const Derived& other, ili javna funkcija u Base.
class Base {
public:
    virtual ~Base() = default;

protected:
    int value_ = 0;
};

class Derived : public Base {
public:
    int peekOther(const Base& other) const { return other.value_; }
};

int main() {
    Derived a;
    Derived b;
    return a.peekOther(b);
}
