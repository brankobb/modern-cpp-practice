// Rešenje zadatka ex1_parameters.

#include <cstddef>
#include <iostream>

struct Sensor {
    const char* name;
    double temp;
};

// Korak 1: referenca kad argument MORA da postoji, pokazivač kad sme da ga
// nema (nullptr) -- tada je provera deo ugovora funkcije.
void swapValues(int& a, int& b) {
    int t = a;
    a = b;
    b = t;
}

bool swapPtr(int* a, int* b) {
    if (a == nullptr || b == nullptr) return false;
    swapValues(*a, *b);
    return true;
}

// Korak 2: [first, last) -- last pokazuje JEDAN IZA kraja (sme da se
// napravi i poredi, ne sme da se dereferencira). const int*: funkcija
// obećava da ne menja elemente, pa prima i const nizove.
int sum(const int* first, const int* last) {
    int s = 0;
    for (const int* p = first; p != last; ++p) s += *p;
    return s;
}

// Korak 3: rezultat možda ne postoji -> pokazivač (nullptr = "nema").
// Referenca bi morala da se veže za nešto i kad je niz prazan.
const Sensor* hottest(const Sensor* arr, std::size_t n) {
    if (n == 0) return nullptr;
    const Sensor* best = arr;
    for (const Sensor* p = arr + 1; p != arr + n; ++p)
        if (p->temp > best->temp) best = p;
    return best;
}

int main() {
    int x = 1, y = 2;
    swapValues(x, y);
    std::cout << "swapValues: x=" << x << " y=" << y << '\n';
    bool ok = swapPtr(&x, &y);
    std::cout << "swapPtr: " << ok << " x=" << x << " y=" << y << '\n';
    std::cout << "swapPtr(nullptr): " << swapPtr(&x, nullptr) << '\n';

    int arr[] = {1, 2, 3, 4, 5};
    std::cout << "sum of all: " << sum(arr, arr + 5) << '\n';
    std::cout << "sum [1, 3): " << sum(arr + 1, arr + 3) << '\n';
    std::cout << "sum of empty: " << sum(arr, arr) << '\n';

    Sensor s[] = {{"engine", 71.5}, {"battery", 38.0}, {"cpu", 84.25}};
    if (const Sensor* p = hottest(s, 3))
        std::cout << "hottest: " << p->name << ' ' << p->temp << '\n';
    if (hottest(s, 0) == nullptr)
        std::cout << "empty array: no result\n";
}
