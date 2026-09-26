// EXPECT-GCC: assignment of read-only location
// EXPECT-CLANG: cannot assign to return value because function 'operator[]' returns a const value
// POGREŠNO: string_view je pogled SAMO ZA ČITANJE -- operator[] vraća
// const char&. Može da gleda i u literal, koji se ne sme menjati.
// Ispravno: menjaj original (std::string), ili napravi kopiju:
// std::string s(sv); s[0] = 'T';
#include <string_view>
int main() {
    std::string_view sv = "temp";
    sv[0] = 'T';
    return sv[0];
}
