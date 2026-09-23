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
// ./check_cases.sh week3-advanced/s18-algorithms  proverava oba.

template <typename C>
void ispisi(const char* opis, const C& c) {
    std::cout << opis << ':';
    for (const auto& x : c) std::cout << ' ' << x;
    std::cout << '\n';
}

// ---------------------------------------------------------------- 1
// Poređenje koje se broji: tako se "vidi" složenost.
long poredjenja = 0;
struct Manje {
    bool operator()(int a, int b) const {
        ++poredjenja;
        return a < b;
    }
};

void sekcija1() {
    std::cout << "\n== 1. složenost: broj poređenja za 1024 elementa\n";
    std::vector<int> v(1024);
    std::iota(v.begin(), v.end(), 0);   // 0, 1, ..., 1023 -- sortirano

    poredjenja = 0;
    std::find_if(v.begin(), v.end(), [](int x) {
        ++poredjenja;
        return x == 1000;
    });
    std::cout << "linearno (find_if) do 1000: " << poredjenja << " -- O(n)\n";

    poredjenja = 0;
    std::binary_search(v.begin(), v.end(), 1000, Manje{});
    std::cout << "binary_search: " << poredjenja << " -- O(log n), log2(1024) = 10\n";

    std::set<int, Manje> s(v.begin(), v.end());
    poredjenja = 0;
    s.find(1000);
    std::cout << "set::find: " << poredjenja << " -- O(log n)\n";

    std::vector<int> r(1000);
    for (int i = 0; i < 1000; ++i) r[i] = (i * 7919) % 1000;   // izmešano
    poredjenja = 0;
    std::sort(r.begin(), r.end(), Manje{});
    std::cout << "sort 1000 elemenata: " << poredjenja << " -- O(n log n), n*log2(n) ~ 9966\n";
}

// ---------------------------------------------------------------- 2
void sekcija2() {
    std::cout << "\n== 2. algoritmi koji ne menjaju opseg\n";
    std::vector<int> v{4, 8, 15, 16, 23, 42};
    auto neparan = [](int x) { return x % 2 != 0; };
    std::cout << "find 16 na poziciji " << std::distance(v.begin(), std::find(v.begin(), v.end(), 16))
              << ", count_if neparnih " << std::count_if(v.begin(), v.end(), neparan) << '\n';
    std::cout << "all_of > 0: " << std::all_of(v.begin(), v.end(), [](int x) { return x > 0; })
              << ", any_of neparan: " << std::any_of(v.begin(), v.end(), neparan)
              << ", none_of > 100: " << std::none_of(v.begin(), v.end(), [](int x) { return x > 100; })
              << '\n';
    auto [mn, mx] = std::minmax_element(v.begin(), v.end());
    std::cout << "min " << *mn << ", max " << *mx << ", zbir " << std::accumulate(v.begin(), v.end(), 0)
              << ", proizvod prva tri " << std::accumulate(v.begin(), v.begin() + 3, 1, std::multiplies<int>())
              << '\n';
    std::vector<int> w{4, 8, 15, 99, 23, 42};
    auto [a, b] = std::mismatch(v.begin(), v.end(), w.begin());
    std::cout << "equal: " << std::equal(v.begin(), v.end(), w.begin()) << ", prva razlika: " << *a << " vs "
              << *b << '\n';
}

// ---------------------------------------------------------------- 3
void sekcija3() {
    std::cout << "\n== 3. algoritmi koji menjaju opseg\n";
    std::vector<int> v{3, 1, 4, 1, 5, 9, 2, 6};
    std::vector<int> parni;
    std::copy_if(v.begin(), v.end(), std::back_inserter(parni), [](int x) { return x % 2 == 0; });
    ispisi("copy_if parni (back_inserter)", parni);
    std::vector<int> kvadrati(v.size());             // odredište MORA da ima mesta (ub/u01)
    std::transform(v.begin(), v.end(), kvadrati.begin(), [](int x) { return x * x; });
    ispisi("transform kvadrati", kvadrati);
    std::replace_if(v.begin(), v.end(), [](int x) { return x > 5; }, 0);
    ispisi("replace_if > 5 -> 0", v);
    // remove ne briše -- premesti zadržane napred i vrati novi kraj (zadatak z3).
    v.erase(std::remove(v.begin(), v.end(), 0), v.end());
    ispisi("erase(remove(0))", v);
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());   // unique: samo susedni duplikati
    ispisi("sort + erase(unique)", v);
    std::reverse(v.begin(), v.end());
    ispisi("reverse", v);
    std::rotate(v.begin(), v.begin() + 1, v.end());
    ispisi("rotate za 1", v);
}

// ---------------------------------------------------------------- 4
void sekcija4() {
    std::cout << "\n== 4. sortiranje i sortirani opsezi\n";
    // stable_sort čuva redosled jednakih.
    std::vector<std::pair<std::string, int>> ocene{{"Ana", 9}, {"Bora", 7}, {"Cane", 9}, {"Dara", 7}};
    std::stable_sort(ocene.begin(), ocene.end(), [](const auto& x, const auto& y) { return x.second > y.second; });
    std::cout << "stable_sort po oceni:";
    for (const auto& [ime, o] : ocene) std::cout << ' ' << ime << '(' << o << ')';
    std::cout << '\n';

    std::vector<int> v{9, 3, 7, 1, 8, 2, 6};
    std::partial_sort(v.begin(), v.begin() + 3, v.end());   // samo prva 3 sortirana
    std::cout << "partial_sort, najmanja 3: " << v[0] << ' ' << v[1] << ' ' << v[2] << '\n';
    std::vector<int> m{9, 3, 7, 1, 8, 2, 6};
    std::nth_element(m.begin(), m.begin() + 3, m.end());    // medijana na mestu 3, O(n)
    std::cout << "nth_element: medijana " << m[3] << '\n';

    std::vector<int> s{1, 3, 3, 3, 5, 8};
    auto [od, doKraja] = std::equal_range(s.begin(), s.end(), 3);
    std::cout << "equal_range(3): pozicije " << std::distance(s.begin(), od) << " do "
              << std::distance(s.begin(), doKraja) << ", lower_bound(4) -> "
              << *std::lower_bound(s.begin(), s.end(), 4) << '\n';
    std::vector<int> a{1, 2, 4, 6}, b{2, 3, 6, 7}, presek, spoj;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(presek));
    std::merge(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(spoj));
    ispisi("set_intersection", presek);
    ispisi("merge", spoj);
}

// ---------------------------------------------------------------- 5
struct Senzor {
    std::string ime;
    int kanal;
    Senzor(std::string i, int k) : ime(std::move(i)), kanal(k) {}
};

void sekcija5() {
    std::cout << "\n== 5. izmene kontejnera u C++11 (i C++17)\n";
    std::vector<int> v{1, 2, 3};                      // initializer_list konstruktor
    std::map<std::string, int> m{{"a", 1}, {"b", 2}};
    std::vector<Senzor> senzori;
    senzori.reserve(2);
    senzori.emplace_back("temp", 1);                  // pravi na mestu, iz argumenata konstruktora
    Senzor& poslednji = senzori.emplace_back("vlaga", 2);   // C++17: vraća referencu
    std::cout << "emplace_back: " << senzori.size() << " senzora, poslednji " << poslednji.ime << '\n';
    std::string dug(40, 'x');
    std::vector<std::string> tekstovi;
    tekstovi.push_back(std::move(dug));               // push_back(T&&): premesti, ne kopira
    std::cout << "posle push_back(std::move): dug.empty() = " << dug.empty() << '\n';
    // cbegin/cend: const iterator i za ne-const kontejner.
    auto it = v.cbegin();
    std::cout << "*cbegin = " << *it << '\n';
    // Slobodne funkcije rade i za C niz.
    int niz[] = {7, 8, 9};
    std::cout << "std::size(niz) = " << std::size(niz) << ", *std::begin(niz) = " << *std::begin(niz)
              << ", std::data(v)[1] = " << std::data(v)[1] << '\n';
    std::vector<int> veliki(1000);
    veliki.resize(10);
    veliki.shrink_to_fit();                           // zahtev (ne garancija) da capacity padne
    std::cout << "posle shrink_to_fit: size " << veliki.size() << ", capacity " << veliki.capacity()
              << "; mapa ima " << m.size() << " elementa\n";
    std::array<int, 3> a{};
    std::cout << "novi u C++11: array (" << a.size() << " el.), forward_list, unordered_* (s16, s17)\n";
}

int main() {
    std::cout << std::boolalpha;
    sekcija1();
    sekcija2();
    sekcija3();
    sekcija4();
    sekcija5();
}
