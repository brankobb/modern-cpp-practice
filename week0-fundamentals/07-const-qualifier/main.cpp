#include <iostream>

void constPointerVariants() {
    int a = 1, b = 2;

    const int* p1 = &a; // pokazivač na const int
    // *p1 = 5;          // TODO: otkomentariši -- compile error
    p1 = &b;             // OK -- pokazivač sam nije const

    int* const p2 = &a;  // const pokazivač na int
    *p2 = 5;              // OK -- vrednost nije const
    // p2 = &b;           // TODO: otkomentariši -- compile error

    const int* const p3 = &a; // oboje const
    (void)p1; (void)p2; (void)p3;
}

class Cache {
public:
    int getExpensiveValue() const {
        if (!cached_) {
            cached_ = true;
            value_ = 42; // dozvoljeno jer je mutable, iako je funkcija const
        }
        return value_;
    }

private:
    mutable bool cached_ = false;
    mutable int value_ = 0;
};

int main() {
    constPointerVariants();

    Cache c;
    std::cout << c.getExpensiveValue() << "\n"; // radi iako je getter const
}
