// KIND: why
// DEMO-OUT: NAIVE elements: 2, sum: 3
//
// Zadatak 2 -- zašto C niz "zaboravi" veličinu kad ga proslediš (sekcije 2, 3)
// Rešenje: exercises/solutions/ex2_array_as_parameter.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/05-compound-types/exercises/ex2_array_as_parameter.cpp -DNAIVE
//   Niz ima 10 elemenata, a funkcija vidi 2. Pročitaj upozorenje
//   kompajlera (-Wsizeof-array-argument). Parametar "int arr[10]" je u
//   stvari "int* arr" -- [10] se ignoriše ([dcl.fct]: niz kao parametar se
//   prilagođava u pokazivač). sizeof(int*) / sizeof(int) = 8 / 4 = 2.
// Korak 2: napiši template <std::size_t N> int sumRef(const int (&arr)[N])
//   -- referenca na niz NE raspada se, pa N kompajler izvede sam.
// Korak 3: napiši int sumArray(const std::array<int, 10>& arr) -- isto,
//   sa std::array. Otkomentariši test.

#include <array>
#include <cstddef>
#include <iostream>

#ifdef NAIVE
int sum(int arr[10]) {
    std::size_t n = sizeof(arr) / sizeof(arr[0]);   // sizeof POKAZIVAČA!
    int s = 0;
    for (std::size_t i = 0; i < n; ++i) s += arr[i];
    std::cout << "elements: " << n << ", sum: " << s << '\n';
    return s;
}
#endif

// TODO korak 2 i 3

int main() {
    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    (void)arr;
#ifdef NAIVE
    sum(arr);
#endif
    // Korak 2 -- otkomentariši:
    // std::cout << "sumRef: " << sumRef(arr) << '\n';

    // Korak 3 -- otkomentariši:
    // std::array<int, 10> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    // std::cout << "sumArray: " << sumArray(a) << '\n';
}

/* EXPECTED OUTPUT
sumRef: 55
sumArray: 55
*/
