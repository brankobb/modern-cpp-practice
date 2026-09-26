#include <algorithm>
#include <atomic>
#include <execution>
#include <functional>
#include <iostream>
#include <numeric>
#include <type_traits>
#include <vector>

// Paralelni algoritmi (C++17, <execution>) -- ISPRAVNI slučajevi.
// Sve se kompajlira bez upozorenja i radi bez ASan/UBSan prijava (g++ 13
// i clang 18, C++17 i C++20). Brojevi sekcija prate notes.md.
// Izlaz je isti bez obzira na to da li je TBB instaliran: sa TBB-om
// libstdc++ radi paralelno (build.sh tada sam doda -ltbb), bez njega
// sekvencijalno. Vremena nisu u izlazu (zavise od mašine) -- izmerena
// su u notes.md, sekcija 5.
// POGREŠNI slučajevi:
//   errors/   -- kod koji se NE kompajlira
//   runtime/  -- izuzetak iz paralelnog algoritma: terminate i uz try
// ./check_cases.sh 8-concurrency/41-parallel-algorithms  proverava ih.
// ub/ nema: ThreadSanitizer uz TBB prijavljuje data race i za ispravan
// kod (notes.md, sekcija 3).

namespace ex = std::execution;

// ---------------------------------------------------------------- 1
void section1() {
    std::cout << "\n== 1. execution policies: the same algorithm, one extra first argument\n";
    std::vector<int> v(100000);
    for (std::size_t i = 0; i < v.size(); ++i) v[i] = static_cast<int>((i * 7919) % 100000);   // izmešano

    auto a = v, b = v, c = v;
    std::sort(ex::seq, a.begin(), a.end());               // sekvencijalno, kao bez politike
    std::sort(ex::par, b.begin(), b.end());               // sme u više niti
    std::sort(ex::par_unseq, c.begin(), c.end());         // više niti + vektorske instrukcije
    std::cout << "sort seq == par == par_unseq: " << (a == b && b == c) << ", first " << b.front() << ", last "
              << b.back() << '\n';

    std::vector<long> squares(v.size());
    std::transform(ex::par, v.begin(), v.end(), squares.begin(), [](int x) { return 1L * x * x; });
    auto evens = std::count_if(ex::par, v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    auto where = std::find(ex::par, v.begin(), v.end(), 42);
    std::cout << "transform(par): squares[3] = " << squares[3] << ", count_if(par) evens = " << evens
              << ", find(par, 42) at position " << (where - v.begin()) << '\n';

    static_assert(std::is_execution_policy_v<std::decay_t<decltype(ex::par)>>);
    std::cout << "is_execution_policy_v<parallel_policy>: true\n";
}

// ---------------------------------------------------------------- 2
void section2() {
    std::cout << "\n== 2. new numeric algorithms: reduce, transform_reduce, scan\n";
    std::vector<int> v(1000);
    std::iota(v.begin(), v.end(), 1);
    std::cout << "accumulate " << std::accumulate(v.begin(), v.end(), 0L) << ", reduce(par) "
              << std::reduce(ex::par, v.begin(), v.end(), 0L) << '\n';

    std::vector<double> x{1.0, 2.0, 3.0}, y{4.0, 5.0, 6.0};
    double dot = std::transform_reduce(ex::par, x.begin(), x.end(), y.begin(), 0.0);   // sum x[i] * y[i]
    long sumOfSquares = std::transform_reduce(ex::par, v.begin(), v.begin() + 10, 0L, std::plus<>{},
                                              [](int e) { return 1L * e * e; });
    std::cout << "transform_reduce: dot product " << dot << ", sum of squares 1..10 " << sumOfSquares << '\n';

    std::vector<int> d{3, 1, 4, 1, 5}, incl(d.size()), excl(d.size()), ps(d.size());
    std::inclusive_scan(ex::par, d.begin(), d.end(), incl.begin());          // i-ti = zbir do i, uključujući
    std::exclusive_scan(ex::par, d.begin(), d.end(), excl.begin(), 0);       // i-ti = zbir do i, bez njega
    std::partial_sum(d.begin(), d.end(), ps.begin());                        // stari C++98 = inclusive_scan
    auto print = [](const char* label, const std::vector<int>& r) {
        std::cout << label << ':';
        for (int e : r) std::cout << ' ' << e;
        std::cout << '\n';
    };
    print("inclusive_scan", incl);
    print("exclusive_scan", excl);
    print("partial_sum   ", ps);
}

// ---------------------------------------------------------------- 3
void section3() {
    std::cout << "\n== 3. shared state: how to avoid a data race\n";
    std::vector<int> v(100000, 1);
    // ❌ for_each(par, ..., [&](int x) { zbir += x; }) je data race (notes.md, sekcija 3).
    // ✅ 1: algoritam koji sam skuplja rezultat
    long sum = std::reduce(ex::par, v.begin(), v.end(), 0L);
    // ✅ 2: std::atomic -- ispravno, ali sve niti se bore za istu promenljivu
    std::atomic<long> atomicSum{0};
    std::for_each(ex::par, v.begin(), v.end(), [&](int x) { atomicSum += x; });
    // ✅ 3: svaki element piše samo u SVOJE mesto
    std::vector<int> doubled(v.size());
    std::transform(ex::par, v.begin(), v.end(), doubled.begin(), [](int e) { return 2 * e; });
    std::cout << "reduce " << sum << ", atomic " << atomicSum.load() << ", transform into its own slot: sum "
              << std::reduce(ex::par, doubled.begin(), doubled.end(), 0L) << '\n';
}

// ---------------------------------------------------------------- 4
void section4() {
    std::cout << "\n== 4. rules: order, associativity, exceptions\n";
    // reduce sme da grupiše i premešta: operacija mora biti asocijativna i komutativna.
    std::vector<int> v{10, 3, 2, 1};
    auto minus = [](int a, int b) { return a - b; };
    std::cout << "100 - 10 - 3 - 2 - 1: accumulate " << std::accumulate(v.begin(), v.end(), 100, minus)
              << ", reduce(seq) " << std::reduce(ex::seq, v.begin(), v.end(), 100, minus) << " (libstdc++; exercise ex2)\n";

    // for_each(par) nema redosled; brojanje ide preko count_if (errors/e01).
    std::vector<int> m{5, -1, 7, -3, 2};
    std::cout << "count_if(par) positive: " << std::count_if(ex::par, m.begin(), m.end(), [](int x) { return x > 0; })
              << '\n';
    // Izuzetak iz lambde u paralelnom algoritmu: std::terminate, try ne pomaže (runtime/r01).
    // Zato provera PRE algoritma:
    bool valid = std::none_of(ex::par, m.begin(), m.end(), [](int x) { return x < -100; });
    std::cout << "check before processing (none_of): " << valid << '\n';
}

int main() {
    std::cout << std::boolalpha;
    section1();
    section2();
    section3();
    section4();
}
