// STD: c++17
// EXPECT-GCC: invalid 'static_cast' from type 'int*' to type 'long int'
// EXPECT-CLANG: static_cast from 'int *' to 'long' is not allowed
// POGREŠNO: static_cast pokazivača u ceo broj.
// Zašto: adresa kao broj zavisi od platforme (veličina pokazivača,
//   raspored memorije), pa to nije "proverena" konverzija. Za nju postoji
//   reinterpret_cast, koji u kodu jasno kaže "ovo je nisko i neprenosivo".
// Ispravno: reinterpret_cast<std::uintptr_t>(p) -- uintptr_t je dovoljno
//   velik za svaki pokazivač (main.cpp, sekcija 3).
int main() {
    int x = 5;
    int* p = &x;
    long addr = static_cast<long>(p);
    return addr != 0;
}
