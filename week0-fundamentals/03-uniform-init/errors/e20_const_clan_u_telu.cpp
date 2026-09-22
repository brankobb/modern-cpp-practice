// STD: c++17
// EXPECT-GCC: uninitialized const member
// EXPECT-CLANG: must explicitly initialize the const member
// POGREŠNO: const član mora biti inicijalizovan u init listi -- u telu
// konstruktora je već "gotov" i dodela nije dozvoljena.
// Ispravno: A(int v) : c_{v} {}
struct A {
    A(int v) { c_ = v; }
    const int c_;
};
int main() {
    A a(1);
    (void)a;
}
