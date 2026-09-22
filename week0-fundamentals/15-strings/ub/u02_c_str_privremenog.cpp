// EXPECT-UB: heap-use-after-free
// POGREŠNO: c_str() privremenog std::string-a sačuvan u promenljivu.
// Zašto: userName() vraća privremeni string koji nestaje na kraju te linije.
//   c_str() nije referenca, pa ne produžava život privremenog (s01).
//   clang -Wall upozori (-Wdangling-gsl), g++ ne.
// Ispravno: std::string name = userName(); pa name.c_str() kad zatreba.
//   Direktno u istom izrazu je bezbedno: std::puts(userName().c_str());
#include <cstdio>
#include <string>

std::string userName() { return std::string(40, 'u'); }

int main() {
    const char* name = userName().c_str();
    std::printf("%c\n", name[0]);
}
