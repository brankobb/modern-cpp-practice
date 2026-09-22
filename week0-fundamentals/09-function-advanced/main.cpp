#include <iostream>

void print(int x) { std::cout << "print(int): " << x << "\n"; }
void print(double x) { std::cout << "print(double): " << x << "\n"; }

void greet(std::string name, int times = 1) {
    for (int i = 0; i < times; ++i) std::cout << "Hi " << name << "\n";
}
// void greet(std::string name); // TODO: otkomentariši -- greet("x") postaje
// dvosmislen poziv (ovaj overload vs greet(name, 1) default) -- compile error

inline int square(int x) { return x * x; } // bezbedno u header-u, bez ODR problema

using BinaryOp = int (*)(int, int); // čitljiviji nego "int (*)(int, int)" inline
int add(int a, int b) { return a + b; }
int mul(int a, int b) { return a * b; }

namespace mymath {
    struct Vec2 { double x, y; };
    double length(const Vec2& v) { return v.x + v.y; } // pojednostavljeno
}

int main() {
    print(5);     // print(int)
    print(5.0);   // print(double)

    greet("Ana", 2);

    std::cout << square(6) << "\n";

    BinaryOp op = add;
    std::cout << "op(2,3)=" << op(2, 3) << "\n";
    op = mul;
    std::cout << "op(2,3)=" << op(2, 3) << "\n";

    mymath::Vec2 v{3, 4};
    // ADL: length(v) bi radio i BEZ mymath:: prefiksa kad se poziva sa
    // argumentom iz mymath namespace-a, ali eksplicitno je jasnije:
    std::cout << "length=" << mymath::length(v) << "\n";
}
