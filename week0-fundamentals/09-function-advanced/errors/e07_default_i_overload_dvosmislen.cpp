// STD: c++17
// EXPECT-GCC: call of overloaded 'f()' is ambiguous
// EXPECT-CLANG: call to 'f' is ambiguous
// POGREŠNO: f() može da znači f() ILI f(int) sa podrazumevanom vrednošću.
// Svaka deklaracija za sebe je ispravna -- greška je tek na mestu poziva.
void f() {}
void f(int = 0) {}
int main() {
    f();
}
