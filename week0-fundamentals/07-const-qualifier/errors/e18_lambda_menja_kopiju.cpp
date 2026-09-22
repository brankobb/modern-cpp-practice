// STD: c++17
// EXPECT-GCC: increment of read-only variable 'x'
// EXPECT-CLANG: captured by copy in a non-mutable lambda
// POGREŠNO: operator() lambde je podrazumevano const, pa lambda ne sme da
// menja kopije koje je uhvatila po vrednosti.
// Ispravno: [x]() mutable { return ++x; }  (menja SVOJU kopiju, ne spoljni x)
int main() {
    int x = 0;
    auto next = [x]() { return ++x; };
    return next();
}
