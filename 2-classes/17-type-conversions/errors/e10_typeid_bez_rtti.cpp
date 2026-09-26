// STD: c++17
// FLAGS: -fno-rtti
// EXPECT-GCC: cannot use 'typeid' with '-fno-rtti'
// EXPECT-CLANG: use of typeid requires -frtti
// POGREŠNO: typeid u programu kompajliranom sa -fno-rtti.
// Zašto: RTTI (run-time type information) su podaci o tipu koje kompajler
//   ugradi uz svaku vtable: ime tipa i veze sa baznim klasama. typeid i
//   dynamic_cast ih čitaju. Embedded projekti i neke biblioteke (LLVM,
//   Chromium) isključuju RTTI da bi smanjili program; tada ni typeid ni
//   dynamic_cast na polimorfnim tipovima ne postoje.
// Ispravno: virtual funkcija koja vraća vrstu (virtual Kind kind() const),
//   ili std::variant umesto hijerarhije. Ili ne isključuj RTTI ako ti treba.
#include <typeinfo>

struct Shape {
    virtual ~Shape() = default;
};

int main() {
    Shape s;
    return typeid(s) == typeid(Shape);
}
