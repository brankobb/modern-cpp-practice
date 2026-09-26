// STD: c++17
// EXPECT-GCC: non-constant condition for static assertion
// EXPECT-CLANG: static assertion expression is not an integral constant expression
// POGREŠNO: static_assert čiji uslov deli nulom.
// Zašto: deljenje nulom je UB, pa izraz nije konstantan i static_assert ne
//   može da ga izračuna. Obe poruke kažu "nije konstantan izraz", a uzrok
//   je u napomeni ispod (division by zero).
// Ispravno: proveri b != 0 u funkciji, ili ne zovi je sa 0.
constexpr int ratio(int a, int b) { return a / b; }

static_assert(ratio(10, 0) == 0, "deljenje");

int main() {}
