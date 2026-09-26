// EXPECT-UB: stack-buffer-overflow
// UB: pokazivač "jedan iza kraja" (arr + 3) sme da se NAPRAVI i da se
// POREDI (tako rade end() iteratori), ali NE sme da se dereferencira.
#include <iostream>
int main() {
    int arr[3] = {1, 2, 3};
    int* end = arr + 3;
    std::cout << *end << "\n";
}
