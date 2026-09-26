// STD: c++17
// EXPECT-GCC: 'double' is not a valid type for a template non-type parameter
// EXPECT-CLANG: a non-type template parameter cannot have type 'double' before C++20
// POGREŠNO do C++20: ne-tipski parametar može biti celobrojni, enum,
// pokazivač, referenca ili nullptr_t -- ali ne double. (C++20 dozvoljava
// i double i jednostavne klase.)
// Ispravno u C++17: parametar funkcije (double scale(double x, double f)),
// ili celobrojni parametar u manjim jedinicama (npr. promili).
template <double Factor>
double scale(double x) { return x * Factor; }
int main() { return static_cast<int>(scale<2.0>(1.0)); }
