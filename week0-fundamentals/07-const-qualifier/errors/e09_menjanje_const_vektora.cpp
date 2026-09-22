// STD: c++17
// EXPECT-GCC: as 'this' argument discards qualifiers
// EXPECT-CLANG: no matching member function for call to 'push_back'
// POGREŠNO: const std::vector -- ni elementi ni veličina se ne menjaju.
#include <vector>
int main() {
    const std::vector<int> v{1, 2, 3};
    v.push_back(4);
}
