// STD: c++17
// EXPECT-GCC: 'Engine' is an inaccessible base of 'Car'
// EXPECT-CLANG: cannot cast 'Car' to its private base class 'Engine'
// POGREŠNO: Car* / Car& koristi se kao Engine& spolja, a nasleđivanje je private.
// Zašto: private nasleđivanje nije "is-a" nego "implementirano pomoću"
//   (EC++ Item 39): Car koristi Engine iznutra, ali spolja nije Engine.
//   Zato konverzija u bazu nije dostupna van klase.
// Ispravno: ako Car jeste Engine, public nasleđivanje; ako samo koristi
//   Engine, kompozicija (član Engine engine_;, EC++ Item 38).
class Engine {
public:
    void start() {}
};

class Car : private Engine {
public:
    void drive() { start(); }
};

int main() {
    Car c;
    Engine& e = c;
    e.start();
}
