#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

// Složenost, algoritmi i izmene kontejnera u C++11 -- ISPRAVNI slučajevi.
// Sve se kompajlira bez upozorenja i radi bez ASan/UBSan prijava (g++ 13
// i clang 18, C++17 i C++20). Brojevi sekcija prate notes.md. Tačan broj
// poređenja za set::find i sort zavisi od biblioteke (ovde libstdc++).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 7-standard-library/36-algorithms  proverava oba.

template <typename C>
void print(const char* label, const C& c) {
    std::cout << label << ':';
    for (const auto& x : c) std::cout << ' ' << x;
    std::cout << '\n';
}

// ---------------------------------------------------------------- 1
// Poređenje koje se broji: tako se "vidi" složenost.
long comparisons = 0;
struct Less {
    bool operator()(int a, int b) const {
        ++comparisons;
        return a < b;
    }
};

void section1() {
    std::cout << "\n== 1. complexity: comparison count for 1024 elements\n";
    std::vector<int> v(1024);
    std::iota(v.begin(), v.end(), 0);   // 0, 1, ..., 1023 -- sortirano

    comparisons = 0;
    std::find_if(v.begin(), v.end(), [](int x) {
        ++comparisons;
        return x == 1000;
    });
    std::cout << "linear (find_if) up to 1000: " << comparisons << " -- O(n)\n";

    comparisons = 0;
    std::binary_search(v.begin(), v.end(), 1000, Less{});
    std::cout << "binary_search: " << comparisons << " -- O(log n), log2(1024) = 10\n";

    std::set<int, Less> s(v.begin(), v.end());
    comparisons = 0;
    s.find(1000);
    std::cout << "set::find: " << comparisons << " -- O(log n)\n";

    std::vector<int> r(1000);
    for (int i = 0; i < 1000; ++i) r[i] = (i * 7919) % 1000;   // izmešano
    comparisons = 0;
    std::sort(r.begin(), r.end(), Less{});
    std::cout << "sort 1000 elements: " << comparisons << " -- O(n log n), n*log2(n) ~ 9966\n";
}

// ---------------------------------------------------------------- 2
void section2() {
    std::cout << "\n== 2. algorithms that do not modify the range\n";
    std::vector<int> v{4, 8, 15, 16, 23, 42};
    auto isOdd = [](int x) { return x % 2 != 0; };
    std::cout << "find 16 at position " << std::distance(v.begin(), std::find(v.begin(), v.end(), 16))
              << ", count_if odd " << std::count_if(v.begin(), v.end(), isOdd) << '\n';
    std::cout << "all_of > 0: " << std::all_of(v.begin(), v.end(), [](int x) { return x > 0; })
              << ", any_of odd: " << std::any_of(v.begin(), v.end(), isOdd)
              << ", none_of > 100: " << std::none_of(v.begin(), v.end(), [](int x) { return x > 100; })
              << '\n';
    auto [mn, mx] = std::minmax_element(v.begin(), v.end());
    std::cout << "min " << *mn << ", max " << *mx << ", sum " << std::accumulate(v.begin(), v.end(), 0)
              << ", product of the first three " << std::accumulate(v.begin(), v.begin() + 3, 1, std::multiplies<int>())
              << '\n';
    std::vector<int> w{4, 8, 15, 99, 23, 42};
    auto [a, b] = std::mismatch(v.begin(), v.end(), w.begin());
    std::cout << "equal: " << std::equal(v.begin(), v.end(), w.begin()) << ", first difference: " << *a << " vs "
              << *b << '\n';
}

// ---------------------------------------------------------------- 3
void section3() {
    std::cout << "\n== 3. algorithms that modify the range\n";
    std::vector<int> v{3, 1, 4, 1, 5, 9, 2, 6};
    std::vector<int> evens;
    std::copy_if(v.begin(), v.end(), std::back_inserter(evens), [](int x) { return x % 2 == 0; });
    print("copy_if evens (back_inserter)", evens);
    std::vector<int> squares(v.size());             // odredište MORA da ima mesta (ub/u01)
    std::transform(v.begin(), v.end(), squares.begin(), [](int x) { return x * x; });
    print("transform squares", squares);
    std::replace_if(v.begin(), v.end(), [](int x) { return x > 5; }, 0);
    print("replace_if > 5 -> 0", v);
    // remove ne briše -- premesti zadržane napred i vrati novi kraj (zadatak ex3).
    v.erase(std::remove(v.begin(), v.end(), 0), v.end());
    print("erase(remove(0))", v);
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());   // unique: samo susedni duplikati
    print("sort + erase(unique)", v);
    std::reverse(v.begin(), v.end());
    print("reverse", v);
    std::rotate(v.begin(), v.begin() + 1, v.end());
    print("rotate by 1", v);
}

// ---------------------------------------------------------------- 4
void section4() {
    std::cout << "\n== 4. sorting and sorted ranges\n";
    // stable_sort čuva redosled jednakih.
    std::vector<std::pair<std::string, int>> grades{{"Ann", 9}, {"Bob", 7}, {"Carl", 9}, {"Dora", 7}};
    std::stable_sort(grades.begin(), grades.end(), [](const auto& x, const auto& y) { return x.second > y.second; });
    std::cout << "stable_sort by grade:";
    for (const auto& [name, o] : grades) std::cout << ' ' << name << '(' << o << ')';
    std::cout << '\n';

    std::vector<int> v{9, 3, 7, 1, 8, 2, 6};
    std::partial_sort(v.begin(), v.begin() + 3, v.end());   // samo prva 3 sortirana
    std::cout << "partial_sort, smallest 3: " << v[0] << ' ' << v[1] << ' ' << v[2] << '\n';
    std::vector<int> m{9, 3, 7, 1, 8, 2, 6};
    std::nth_element(m.begin(), m.begin() + 3, m.end());    // medijana na mestu 3, O(n)
    std::cout << "nth_element: median " << m[3] << '\n';

    std::vector<int> s{1, 3, 3, 3, 5, 8};
    auto [first, last] = std::equal_range(s.begin(), s.end(), 3);
    std::cout << "equal_range(3): positions " << std::distance(s.begin(), first) << " to "
              << std::distance(s.begin(), last) << ", lower_bound(4) -> "
              << *std::lower_bound(s.begin(), s.end(), 4) << '\n';
    std::vector<int> a{1, 2, 4, 6}, b{2, 3, 6, 7}, intersection, merged;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(intersection));
    std::merge(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(merged));
    print("set_intersection", intersection);
    print("merge", merged);
}

// ---------------------------------------------------------------- 5
struct Sensor {
    std::string name;
    int channel;
    Sensor(std::string i, int k) : name(std::move(i)), channel(k) {}
};

void section5() {
    std::cout << "\n== 5. container changes in C++11 (and C++17)\n";
    std::vector<int> v{1, 2, 3};                      // initializer_list konstruktor
    std::map<std::string, int> m{{"a", 1}, {"b", 2}};
    std::vector<Sensor> sensors;
    sensors.reserve(2);
    sensors.emplace_back("temp", 1);                  // pravi na mestu, iz argumenata konstruktora
    Sensor& lastSensor = sensors.emplace_back("humidity", 2);   // C++17: vraća referencu
    std::cout << "emplace_back: " << sensors.size() << " sensors, last " << lastSensor.name << '\n';
    std::string longText(40, 'x');
    std::vector<std::string> texts;
    texts.push_back(std::move(longText));               // push_back(T&&): premesti, ne kopira
    std::cout << "after push_back(std::move): longText.empty() = " << longText.empty() << '\n';
    // cbegin/cend: const iterator i za ne-const kontejner.
    auto it = v.cbegin();
    std::cout << "*cbegin = " << *it << '\n';
    // Slobodne funkcije rade i za C niz.
    int arr[] = {7, 8, 9};
    std::cout << "std::size(arr) = " << std::size(arr) << ", *std::begin(arr) = " << *std::begin(arr)
              << ", std::data(v)[1] = " << std::data(v)[1] << '\n';
    std::vector<int> big(1000);
    big.resize(10);
    big.shrink_to_fit();                           // zahtev (ne garancija) da capacity padne
    std::cout << "after shrink_to_fit: size " << big.size() << ", capacity " << big.capacity()
              << "; the map has " << m.size() << " elements\n";
    std::array<int, 3> a{};
    std::cout << "new in C++11: array (" << a.size() << " el.), forward_list, unordered_* (lessons 34, 35)\n";
}

int main() {
    std::cout << std::boolalpha;
    section1();
    section2();
    section3();
    section4();
    section5();
}
