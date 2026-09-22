#include <iostream>

// Kviz -- predvidi ispis SVAKOG bloka pre pokretanja.

void quiz1_pointers() {
    int a = 5, b = 10;
    int* p = &a;
    p = &b;      // pomera POKAZIVAČ na b, a ostaje 5
    *p = 20;     // menja b, ne a
    std::cout << "Q1: a=" << a << " b=" << b << "\n";
}

void quiz2_pointer_to_pointer() {
    int x = 1;
    int* p = &x;
    int** pp = &p;
    **pp = 99; // menja x kroz duplo dereferenciranje
    std::cout << "Q2: x=" << x << "\n";
}

void quiz3_reference_no_rebind() {
    int a = 1, b = 2;
    int& r = a;
    r = b;       // OVO NIJE rebind -- kopira vrednost b u a!
    b = 100;
    std::cout << "Q3: a=" << a << " b=" << b << " (r i dalje referiše a)\n";
}

int func(int& x) { x += 1; return x; }

void quiz4_reference_param() {
    int n = 5;
    int result = func(n) + func(n); // redosled evaluacije operanada + nije definisan pre C++17
    std::cout << "Q4: n=" << n << " result=" << result << " (probaj da objasniš)\n";
}

int main() {
    quiz1_pointers();
    quiz2_pointer_to_pointer();
    quiz3_reference_no_rebind();
    quiz4_reference_param();
}
