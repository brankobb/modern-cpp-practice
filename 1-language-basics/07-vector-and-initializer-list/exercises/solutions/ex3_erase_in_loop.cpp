// Rešenje zadatka ex3_erase_in_loop.

#include <algorithm>
#include <iostream>
#include <vector>

void print(const std::vector<int>& v) {
    std::cout << "after erasing:";
    for (int x : v) std::cout << ' ' << x;
    std::cout << '\n';
}

// Ako posle v.erase(it) uradiš ++it (nije dobro): it je poništen, a na
// njegovo mesto je već došao sledeći element -- preskočiš ga, a na kraju
// odeš iza end().
// Treba ovako: a) erase vraća iterator na element POSLE obrisanog; tada se
// ne pomera dalje.
void removeNegativesA(std::vector<int>& v) {
    for (auto it = v.begin(); it != v.end();) {
        if (*it < 0)
            it = v.erase(it);
        else
            ++it;
    }
}

// Možeš i ovako: b) erase-remove -- jedan prolaz, bez ručnih iteratora.
void removeNegativesB(std::vector<int>& v) {
    v.erase(std::remove_if(v.begin(), v.end(), [](int x) { return x < 0; }), v.end());
}

int main() {
    std::vector<int> a{1, -2, -3, 4, -5}, b = a;
    removeNegativesA(a);
    removeNegativesB(b);
    print(a);
    print(b);
}
