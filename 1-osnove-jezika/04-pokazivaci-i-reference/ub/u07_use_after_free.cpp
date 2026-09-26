// EXPECT-UB: heap-use-after-free
// UB: korišćenje memorije posle delete. Pokazivač i dalje "pokazuje" na
// staru adresu -- delete ga ne postavlja na nullptr.
// Ispravno: ne koristi posle delete; bolje -- std::unique_ptr (lekcija 32).
#include <iostream>
int main() {
    int* p = new int(42);
    delete p;
    std::cout << *p << "\n";
}
