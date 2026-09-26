// EXPECT-UB: stack-use-after-scope
// UB: produženje životnog veka privremenog objekta NE prolazi kroz funkciju.
// std::max vraća const T& na jedan od argumenata; argumenti 1 i 2 su
// privremeni i nestaju na kraju izraza -> r visi.
// Ispravno: int r = std::max(1, 2);  (kopija, bez reference)
#include <algorithm>
#include <iostream>
int main() {
    const int& r = std::max(1, 2);
    std::cout << r << "\n";
}
