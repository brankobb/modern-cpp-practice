// STD: c++17
// EXPECT-GCC: template declaration of 'typedef'
// EXPECT-CLANG: a typedef cannot be a template
// POGREŠNO (EMC Item 9): typedef ne može biti template.
// Ispravno: template <typename T> using Vec = std::vector<T>;  (alias template)
#include <vector>
template <typename T>
typedef std::vector<T> Vec;
int main() {}
