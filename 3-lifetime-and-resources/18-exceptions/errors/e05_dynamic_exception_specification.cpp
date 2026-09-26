// EXPECT-GCC: ISO C++17 does not allow dynamic exception specifications
// EXPECT-CLANG: ISO C++17 does not allow dynamic exception specifications
// POGREŠNO od C++17: "throw(tip)" posle funkcije (dinamička specifikacija
// iz C++98) je uklonjena iz jezika. Proveravala se tek pri izvršavanju, i
// niko je nije pravilno koristio. (throw() bez tipova je u C++17 bio
// sinonim za noexcept, a u C++20 je i on uklonjen.)
// Ispravno: noexcept (ne baca) ili ništa (može da baci).
void send(int byte) throw(int);
int main() {}
