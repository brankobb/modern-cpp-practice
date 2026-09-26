// EXPECT-GCC: deduced conflicting types for parameter 'T' ('int' and 'double')
// EXPECT-CLANG: deduced conflicting types for parameter 'T' ('int' vs. 'double')
// POGREŠNO: iz prvog argumenta T = int, iz drugog T = double. Dedukcija
// ne pravi konverzije da bi pomirila tipove -- svaki argument mora da da
// ISTI T.
// Ispravno: eksplicitan argument maks<double>(1, 2.5) (tada su dozvoljene
// obične konverzije), ili isti tip argumenata maks(1.0, 2.5), ili šablon
// sa dva parametra i povratnim tipom std::common_type_t<A, B>.
template <typename T>
T maks(T a, T b) { return b < a ? a : b; }
int main() { return maks(1, 2.5) > 0; }
