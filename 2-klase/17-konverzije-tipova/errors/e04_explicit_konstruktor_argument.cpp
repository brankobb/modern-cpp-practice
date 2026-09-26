// STD: c++17
// EXPECT-GCC: could not convert '5.0e+0' from 'double' to 'Meters'
// EXPECT-CLANG: no matching function for call to 'twice'
// POGREŠNO: prosleđivanje double tamo gde se očekuje Meters sa explicit
//   konstruktorom.
// Zašto: prosleđivanje argumenta je copy inicijalizacija parametra, a ona
//   ne koristi explicit konstruktore (lekcija 03). To je i poenta: 5.0 može
//   biti metri, stope ili sekunde, pa pozivalac mora da kaže šta je.
// Ispravno: twice(Meters(5.0)) ili twice(Meters{5.0}).
class Meters {
public:
    explicit Meters(double v) : v_(v) {}
    double value() const { return v_; }

private:
    double v_;
};

double twice(Meters m) { return 2 * m.value(); }

int main() {
    return static_cast<int>(twice(5.0));
}
