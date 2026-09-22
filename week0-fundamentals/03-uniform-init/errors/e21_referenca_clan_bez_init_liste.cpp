// STD: c++17
// EXPECT-GCC: uninitialized reference member
// EXPECT-CLANG: must explicitly initialize the reference member
// POGREŠNO: referenca kao član mora biti vezana u init listi.
// Ispravno: B(int& r) : r_{r} {}
struct B {
    B(int& r) { r_ = r; }
    int& r_;
};
int main() {
    int x = 1;
    B b(x);
    (void)b;
}
