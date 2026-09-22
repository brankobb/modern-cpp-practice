#include <iostream>

struct PodPoint { // struct -- default public, nema invarijantu
    int x, y;
};

class Age { // class -- default private, čuva invarijantu (age >= 0)
public:
    explicit Age(int value) { set(value); }

    Age& set(int value) {
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
    static int count_; // deklaracija -- definicija je ispod, van klase
};

int InstanceCounter::count_ = 0; // OBAVEZNA definicija (pre C++17 inline static)

int main() {
    PodPoint p{1, 2}; // direktan pristup -- OK, nema invarijante
    std::cout << "p=(" << p.x << "," << p.y << ")\n";

    Age a(-5); // biće 0 zbog invarijante
    a.set(30).set(40); // method chaining preko this
    std::cout << "a=" << a.get() << "\n";

    std::cout << "count=" << InstanceCounter::count() << "\n"; // 0, nema instance
    {
        InstanceCounter c1, c2, c3;
        std::cout << "count=" << InstanceCounter::count() << "\n"; // 3
    }
    std::cout << "count=" << InstanceCounter::count() << "\n"; // 0, dtor smanjio
}
