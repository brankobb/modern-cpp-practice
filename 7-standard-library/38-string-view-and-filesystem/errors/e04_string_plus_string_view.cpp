// EXPECT-GCC: no match for 'operator+' (operand types are 'std::string' {aka 'std::__cxx11::basic_string<char>'} and 'std::string_view'
// EXPECT-CLANG: invalid operands to binary expression ('std::string' (aka 'basic_string<char>') and 'std::string_view'
// POGREŠNO: C++17 (i C++20) nema operator+ za std::string i string_view
// (dodat tek u C++26).
// Ispravno: s + std::string(sv), ili bez privremenog stringa:
// std::string r = s; r += sv;  (ili r.append(sv))
#include <string>
#include <string_view>
int main() {
    std::string s = "kanal: ";
    std::string_view sv = "temp";
    std::string r = s + sv;
    return static_cast<int>(r.size());
}
