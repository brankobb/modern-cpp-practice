// Rešenje zadatka ex2_reduce_nije_accumulate.

#include <execution>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>

// Ako se reduce da oduzimanje (nije dobro): reduce sme da grupiše i
// premešta, a oduzimanje nije ni asocijativno ni komutativno -- rezultat
// zavisi od biblioteke i politike.
// Treba ovako: paralelno samo ono što sme -- zbir; oduzimanje jednom, na kraju.
int preostalo(int budzet, const std::vector<int>& t) {
    return budzet - std::reduce(std::execution::par, t.begin(), t.end(), 0, std::plus<>{});
}

int main() {
    std::vector<int> troskovi{10, 3, 2, 1};
    std::cout << "preostalo od 100 posle 10, 3, 2 i 1: " << preostalo(100, troskovi) << '\n';
    std::vector<int> mnogo(1000, 1);
    std::cout << "preostalo od 5000 posle 1000 x 1: " << preostalo(5000, mnogo) << '\n';
}
