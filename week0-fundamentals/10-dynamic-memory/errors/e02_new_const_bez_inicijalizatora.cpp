// STD: c++17
// EXPECT-GCC: uninitialized const in 'new' of 'const int'
// EXPECT-CLANG: default initialization of an object of const type 'const int'
// POGREŠNO: new const int bez inicijalizatora.
// Zašto: "new T" radi default inicijalizaciju. Za int to znači neodređenu
//   vrednost, a const objekat posle toga više ne može da dobije vrednost.
//   Isto pravilo kao "const int x;" (lekcija 07).
// Ispravno: new const int(5), ili new const int() za nulu.
int main() {
    const int* p = new const int;
    delete p;
}
