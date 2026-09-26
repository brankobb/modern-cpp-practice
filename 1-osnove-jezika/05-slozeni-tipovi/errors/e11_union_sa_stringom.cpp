// STD: c++17
// EXPECT-GCC: use of deleted function 'Value::Value()'
// EXPECT-CLANG: call to implicitly-deleted default constructor of 'Value'
// POGREŠNO: union sa članom koji ima netrivijalan konstruktor/destruktor
// (std::string) gubi podrazumevani konstruktor i destruktor -- union ne zna
// koji član je aktivan, pa ne zna šta da uništi.
// Ispravno: std::variant<int, std::string>  (zna koji je aktivan)
#include <string>
union Value {
    int i;
    std::string s;
};
int main() {
    Value v;
    v.i = 1;
}
