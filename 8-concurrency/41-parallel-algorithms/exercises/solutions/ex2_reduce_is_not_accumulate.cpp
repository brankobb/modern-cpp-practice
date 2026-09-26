// Rešenje zadatka ex2_reduce_is_not_accumulate.

#include <execution>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>

// Ako se reduce da oduzimanje (nije dobro): reduce sme da grupiše i
// premešta, a oduzimanje nije ni asocijativno ni komutativno -- rezultat
// zavisi od biblioteke i politike.
// Treba ovako: paralelno samo ono što sme -- zbir; oduzimanje jednom, na kraju.
int remaining(int budget, const std::vector<int>& t) {
    return budget - std::reduce(std::execution::par, t.begin(), t.end(), 0, std::plus<>{});
}

int main() {
    std::vector<int> costs{10, 3, 2, 1};
    std::cout << "remaining from 100 after 10, 3, 2 and 1: " << remaining(100, costs) << '\n';
    std::vector<int> many(1000, 1);
    std::cout << "remaining from 5000 after 1000 x 1: " << remaining(5000, many) << '\n';
}
