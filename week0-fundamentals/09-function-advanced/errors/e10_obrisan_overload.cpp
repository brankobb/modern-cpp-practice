// STD: c++17
// EXPECT-GCC: use of deleted function 'bool isLucky(double)'
// EXPECT-CLANG: call to deleted function 'isLucky'
// NAMERNA GREŠKA (EMC Item 11): "= delete" na overload-u zabranjuje
// neželjenu implicitnu konverziju. Bez njega bi isLucky(3.5) tiho postao
// isLucky(3).
bool isLucky(int number) { return number == 7; }
bool isLucky(double) = delete;
int main() {
    return isLucky(3.5);
}
