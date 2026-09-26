// STD: c++20
// EXPECT-GCC: does not have a constant initializer
// EXPECT-CLANG: variable does not have a constant initializer
// POGREŠNO: constinit promenljiva inicijalizovana pozivom obične funkcije.
// Zašto: constinit garantuje STATIČKU inicijalizaciju (pre bilo kakvog
//   koda), što rešava redosled inicijalizacije između fajlova (lekcija 08,
//   ub/u01). compute() nije constexpr, pa bi inicijalizacija bila
//   dinamička, i kompajler to odbije umesto da tiho uvede problem redosleda.
// Ispravno: constexpr int compute(); -- ili ukloni constinit i koristi
//   funkciju sa static lokalnom promenljivom.
int compute() { return 41; }

constinit int base = compute();

int main() {
    return base;
}
