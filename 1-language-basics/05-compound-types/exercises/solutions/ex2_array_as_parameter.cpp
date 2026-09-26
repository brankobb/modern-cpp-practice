// Rešenje zadatka ex2_array_as_parameter.

#include <array>
#include <cstddef>
#include <iostream>

// Ako pišeš "int sum(int arr[10])" (nije dobro): to je "int sum(int*)",
// veličina se izgubi, a sizeof(arr) meri pokazivač.
// Treba ovako: referenca na niz čuva tip int[N], pa N stiže u funkciju.
template <std::size_t N>
int sumRef(const int (&arr)[N]) {
    int s = 0;
    for (int x : arr) s += x;      // range-for radi, jer je tip i dalje niz
    return s;
}

// Možeš i ovako: std::array je objekat, veličina je deo tipa i nikad se ne
// raspada u pokazivač.
int sumArray(const std::array<int, 10>& arr) {
    int s = 0;
    for (int x : arr) s += x;
    return s;
}

int main() {
    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::cout << "sumRef: " << sumRef(arr) << '\n';

    std::array<int, 10> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::cout << "sumArray: " << sumArray(a) << '\n';
}
