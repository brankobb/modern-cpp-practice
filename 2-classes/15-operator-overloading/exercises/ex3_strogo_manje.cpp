// KIND: why
// DEMO-OUT: NAIVE velicina: 3, count\(\{1, 2\}\): 0
// DEMO-UB: NAIVE_SORT heap-buffer-overflow
//
// Zadatak 3 -- zašto operator< mora biti STROG (sekcija 5)
// Rešenje: exercises/solutions/ex3_strogo_manje.cpp
//
// std::set, std::map i std::sort traže da operator< bude "strict weak
// ordering" ([alg.sorting]): između ostalog, a < a mora biti false.
// Naivna verzija koristi <= za minor, pa je v < v tačno.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 2-classes/15-operator-overloading/exercises/ex3_strogo_manje.cpp -DNAIVE
//   Ubačeno je {1, 2} dva puta i {2, 0} jednom: set ima 3 elementa, a
//   count({1, 2}) je 0! set dva elementa smatra jednakim kad
//   !(a < b) && !(b < a) -- sa <= to nikad nije tačno, pa se duplikat
//   ubaci, a traženje ga ne nađe.
// Korak 2: isto pravilo, gora posledica:
//     ./build.sh .../ex3_strogo_manje.cpp -DNAIVE_SORT
//   std::sort 40 jednakih elemenata: ASan prijavi heap-buffer-overflow.
//   libstdc++ u unutrašnjoj petlji ne proverava granice, jer se oslanja
//   na to da a < a nije tačno (zato je kršenje pravila UB, a ne samo
//   "pogrešan redosled").
// Korak 3: u #else grani napiši ispravan operator<: poređenje parova
//   std::tie(a.major, a.minor) < std::tie(b.major, b.minor) (<tuple>) --
//   leksikografski i strogo. Otkomentariši test.
//   (U C++20 je još kraće: auto operator<=>(const Verzija&) const = default;)

#include <algorithm>
#include <iostream>
#include <set>
#include <tuple>
#include <vector>

struct Verzija {
    int major;
    int minor;
};

#if defined(NAIVE) || defined(NAIVE_SORT)
bool operator<(const Verzija& a, const Verzija& b) {
    return a.major < b.major || (a.major == b.major && a.minor <= b.minor);
}
#else
// TODO korak 3
#endif

int main() {
#if defined(NAIVE)
    std::set<Verzija> s{{1, 2}, {1, 2}, {2, 0}};
    std::cout << "velicina: " << s.size() << ", count({1, 2}): " << s.count({1, 2}) << '\n';
#elif defined(NAIVE_SORT)
    std::vector<Verzija> v(40, Verzija{1, 1});
    std::sort(v.begin(), v.end());
    std::cout << "sortirano\n";
#else
    // Korak 3 -- otkomentariši:
    // std::set<Verzija> s{{1, 2}, {1, 2}, {2, 0}};
    // std::cout << "velicina: " << s.size() << ", count({1, 2}): " << s.count({1, 2}) << '\n';
    // std::vector<Verzija> v(40, Verzija{1, 1});
    // v.push_back({0, 9});
    // std::sort(v.begin(), v.end());
    // std::cout << "prvi: " << v.front().major << '.' << v.front().minor << '\n';
#endif
}

/* EXPECTED OUTPUT
velicina: 2, count({1, 2}): 1
prvi: 0.9
*/
