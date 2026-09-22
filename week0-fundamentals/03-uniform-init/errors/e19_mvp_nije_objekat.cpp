// STD: c++17
// EXPECT-GCC: non-class type
// EXPECT-CLANG: base of member reference is a function
// POGREŠNO: most vexing parse -- obj NIJE objekat, nego deklaracija funkcije.
// Zato obj.value ne postoji.
// Ispravno: MyClass obj{};  ili  MyClass obj;
struct MyClass {
    int value = 0;
};
int main() {
    MyClass obj();
    return obj.value;
}
