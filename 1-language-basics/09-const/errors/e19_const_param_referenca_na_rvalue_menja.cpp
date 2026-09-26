// STD: c++17
// EXPECT-GCC: no match for 'operator+='
// EXPECT-CLANG: no viable overloaded '+='
// POGREŠNO: const T& parametar prima i privremene objekte, ali ih ne sme menjati.
#include <string>
void shout(const std::string& s) {
    s += "!";
}
int main() {
    shout("hej");
}
