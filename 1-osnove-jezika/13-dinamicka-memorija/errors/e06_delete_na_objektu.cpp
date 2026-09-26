// STD: c++17
// EXPECT-GCC: type 'int' argument given to 'delete', expected pointer
// EXPECT-CLANG: cannot delete expression of type 'int'
// POGREŠNO: delete na vrednosti koja nije pokazivač.
// Zašto: delete prima pokazivač dobijen od new. Ovde se to vidi pri
//   kompajliranju. delete &x (pokazivač na lokalnu promenljivu) se
//   kompajlira, ali je UB (ub/u04).
// Ispravno: lokalne promenljive se ne brišu; nestaju same na kraju scope-a.
int main() {
    int x = 5;
    delete x;
}
