// STD: c++17
// EXPECT-GCC: 'int Base::secret_' is private within this context
// EXPECT-CLANG: 'secret_' is a private member of 'Base'
// POGREŠNO: izvedena klasa čita private član bazne.
// Zašto: private znači samo za samu klasu, ne i za izvedene. Izvedena klasa
//   SADRŽI secret_ (zauzima memoriju), ali ne sme da ga imenuje.
// Ispravno: protected član, ili (bolje) protected/public funkcija u bazi.
//   protected podaci su slabija enkapsulacija: svaka izvedena klasa može da
//   pokvari invarijantu baze (C.133).
class Base {
public:
    virtual ~Base() = default;

private:
    int secret_ = 1;
};

class Derived : public Base {
public:
    int peek() const { return secret_; }
};

int main() {
    Derived d;
    return d.peek();
}
