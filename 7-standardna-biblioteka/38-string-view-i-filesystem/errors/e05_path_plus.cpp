// EXPECT-GCC: no match for 'operator+' (operand types are 'std::filesystem::__cxx11::path' and 'const char [6]')
// EXPECT-CLANG: invalid operands to binary expression ('std::filesystem::path' and 'const char[6]')
// POGREŠNO: path nema operator+. Spajanje delova putanje je operator /,
// koji sam doda separator ("log" / "a.txt" -> "log/a.txt"); += dodaje
// znakove bez separatora ("log" += ".txt" -> "log.txt").
// Ispravno: auto p = dir / "a.txt";
#include <filesystem>
int main() {
    std::filesystem::path dir = "log";
    auto p = dir + "a.txt";
    return p.empty();
}
