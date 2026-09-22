#include <iostream>

struct PodPoint { // struct -- default public, nema invarijantu
    int x, y;
};

class Age { // class -- default private, čuva invarijantu (age >= 0)
public:
    explicit Age(int value) { set(value); }

    Age& set(int value) {
        // Ako dozvoliš direktan pristup age (NIJE DOBRO, kao public int
        // age;) jer bilo ko može da postavi negativnu vrednost i pokvari
        // invarijantu "starost je >= 0" bez ikakve provere.
        // Treba da čuvaš podatak PRIVATNIM i propustiš izmene kroz
        // funkciju koja proverava invarijantu (kao ovde set()).
        if (value < 0) value = 0; // enkapsulacija: interfejs čuva invarijantu
        value_ = value;
        return *this; // this -- omogućava chaining: age.set(5).set(10);
    }

    int get() const { return value_; }

private:
    int value_;
};

class InstanceCounter {
public:
    InstanceCounter() { ++count_; }
    ~InstanceCounter() { --count_; }

    static int count() { return count_; } // static -- nema this, deli se

private:
    // Ako probaš da pristupiš non-static članu iz static funkcije (NIJE
    // DOBRO, ne kompajlira) jer static funkcija NEMA this -- nema "koju
    // instancu" da pita za non-static podatak.
    // Treba da static funkcija koristi SAMO static članove.
    static int count_; // deklaracija -- definicija je ispod, van klase
};

// Ako zaboraviš ovu definiciju (NIJE DOBRO) jer je gornja linija u klasi
// samo DEKLARACIJA (pre C++17) -- linker javlja "undefined reference".
// Treba da definišeš static član TAČNO JEDNOM van klase, obično u .cpp
// fajlu.
// Možeš i inline static int count_ = 0; DIREKTNO u klasi (C++17+) -- tad
// ova linija ispod nije potrebna.
int InstanceCounter::count_ = 0; // OBAVEZNA definicija (pre C++17 inline static)

int main() {
    std::cout << "-- struct (public default) --\n";
    PodPoint p{1, 2}; // direktan pristup -- OK, nema invarijante
    std::cout << "p=(" << p.x << "," << p.y << ")\n";

    std::cout << "-- class (private default) + this chaining --\n";
    Age a(-5); // biće 0 zbog invarijante
    a.set(30).set(40); // method chaining preko this
    std::cout << "a=" << a.get() << "\n";

    std::cout << "-- static member --\n";
    std::cout << "count=" << InstanceCounter::count() << "\n"; // 0, nema instance
    {
        InstanceCounter c1, c2, c3;
        std::cout << "count=" << InstanceCounter::count() << "\n"; // 3
    }
    std::cout << "count=" << InstanceCounter::count() << "\n"; // 0, dtor smanjio
}
