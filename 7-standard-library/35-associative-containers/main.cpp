#include <algorithm>
#include <cctype>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Asocijativni i neuređeni kontejneri -- ISPRAVNI slučajevi. Sve se
// kompajlira bez upozorenja i radi bez ASan/UBSan prijava (g++ 13 i
// clang 18, C++17 i C++20). Brojevi sekcija prate notes.md. Broj bucket-a
// je za libstdc++; libc++ može da da druge.
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 7-standard-library/35-associative-containers  proverava oba.

template <typename C>
void print(const char* label, const C& c) {
    std::cout << label << ':';
    for (const auto& x : c) std::cout << ' ' << x;
    std::cout << '\n';
}

// ---------------------------------------------------------------- 1
void section1() {
    std::cout << "\n== 1. std::set and std::multiset\n";
    std::set<int> s{5, 1, 4, 1, 3};                 // sortiran, bez duplikata
    print("set", s);
    auto [it, inserted] = s.insert(4);               // već postoji
    std::cout << "insert(4): inserted " << inserted << ", iterator to " << *it << '\n';
    std::cout << "count(3) " << s.count(3) << ", find(7) == end: " << (s.find(7) == s.end()) << '\n';
    std::cout << "lower_bound(2) " << *s.lower_bound(2) << ", upper_bound(4) " << *s.upper_bound(4)
              << '\n';

    std::multiset<int> ms{1, 2, 2, 2, 3};           // sortiran, SA duplikatima
    std::multiset<int> ms2 = ms;
    auto erased = ms.erase(2);                    // briše SVE jednake
    ms2.erase(ms2.find(2));                         // briše JEDAN
    std::cout << "multiset.erase(2) erased " << erased << '\n';
    print("remaining", ms);
    print("multiset.erase(find(2))", ms2);
}

// ---------------------------------------------------------------- 2
struct ByLength {   // poredi po dužini -- "jednaki" su stringovi iste dužine
    bool operator()(const std::string& a, const std::string& b) const { return a.size() < b.size(); }
};

void section2() {
    std::cout << "\n== 2. ordering and equivalence\n";
    std::set<int, std::greater<int>> descending{1, 3, 2};
    print("set<int, greater>", descending);
    // Poredak određuje i šta je "isto": !(a < b) && !(b < a).
    std::set<std::string, ByLength> byLength{"aa", "b", "cc", "ddd"};
    print("set by length (\"cc\" in \"the same\" as \"aa\")", byLength);
    // Lambda kao poredak: tip je decltype(lambda), a objekat se prosledi
    // konstruktoru.
    auto caseInsensitive = [](const std::string& a, const std::string& b) {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(),
                                            [](unsigned char x, unsigned char y) { return std::tolower(x) < std::tolower(y); });
    };
    std::set<std::string, decltype(caseInsensitive)> names(caseInsensitive);
    for (const char* i : {"Ann", "bob", "ANN", "Carl"}) names.insert(i);
    print("ignoring case", names);
}

// ---------------------------------------------------------------- 3
void section3() {
    std::cout << "\n== 3. std::map and std::multimap\n";
    std::map<std::string, int> channels{{"temp", 1}, {"pressure", 2}};
    channels["humidity"] = 3;                             // [] ubaci ako ne postoji
    std::cout << "channels[\"unknown\"] = " << channels["unknown"] << " -- and now size = " << channels.size()
              << '\n';
    // Čitanje bez ubacivanja: find ili at.
    if (auto it = channels.find("temp"); it != channels.end()) std::cout << "find temp: " << it->second << '\n';
    try {
        std::cout << channels.at("missing");
    } catch (const std::out_of_range&) {
        std::cout << "at(\"missing\"): out_of_range\n";
    }
    // insert NE prepisuje postojeći; insert_or_assign i try_emplace (C++17).
    auto [it, inserted] = channels.insert({"temp", 99});
    std::cout << "insert of an existing key: inserted " << inserted << ", value stayed " << it->second << '\n';
    channels.insert_or_assign("temp", 10);
    channels.try_emplace("temp", 77);                  // postoji: ne radi ništa (ni ne pravi vrednost)
    channels.try_emplace("current", 4);
    for (const auto& [name, channel] : channels) std::cout << ' ' << name << '=' << channel;
    std::cout << '\n';

    std::multimap<std::string, int> readings{{"temp", 21}, {"pressure", 1013}, {"temp", 22}, {"temp", 20}};
    auto [first, last] = readings.equal_range("temp");   // svi sa ključem "temp", redom ubacivanja
    std::cout << "temp readings:";
    for (auto i = first; i != last; ++i) std::cout << ' ' << i->second;
    std::cout << " (total temp keys: " << readings.count("temp") << ")\n";
}

// ---------------------------------------------------------------- 4
void section4() {
    std::cout << "\n== 4. unordered containers: hash table\n";
    std::unordered_map<std::string, int> counter;
    for (const char* word : {"a", "b", "a", "c", "a", "b"}) ++counter[word];
    // Redosled iteracije NIJE određen -- za ispis ga sortiramo.
    std::vector<std::pair<std::string, int>> sorted(counter.begin(), counter.end());
    std::sort(sorted.begin(), sorted.end());
    for (const auto& [word, n] : sorted) std::cout << ' ' << word << '=' << n;
    std::cout << '\n';
    std::cout << "size " << counter.size() << ", bucket_count " << counter.bucket_count() << ", load_factor "
              << counter.load_factor() << '\n';
    std::unordered_set<int> u;
    u.reserve(100);                                  // bucket-i za 100 elemenata unapred: bez rehash-a
    std::size_t before = u.bucket_count();
    for (int i = 0; i < 100; ++i) u.insert(i);
    std::cout << "after reserve(100) and 100 inserts bucket_count unchanged: " << (before == u.bucket_count())
              << '\n';
}

// ---------------------------------------------------------------- 5
struct Point {
    int x, y;
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};

// Heš za sopstveni tip: funkcijski objekat koji kombinuje heševe polja.
struct PointHash {
    std::size_t operator()(const Point& t) const noexcept {
        std::size_t h = std::hash<int>{}(t.x);
        return h ^ (std::hash<int>{}(t.y) + 0x9e3779b9 + (h << 6) + (h >> 2));
    }
};

// Loš heš: svi ključevi u isti bucket.
struct BadHash {
    std::size_t operator()(const Point&) const noexcept { return 42; }
};

void section5() {
    std::cout << "\n== 5. std::hash and a custom hash\n";
    std::cout << "std::hash<int>{}(42) == std::hash<int>{}(42): "
              << (std::hash<int>{}(42) == std::hash<int>{}(42)) << '\n';
    std::unordered_set<Point, PointHash> points{{1, 2}, {2, 1}, {3, 4}};
    std::cout << "contains (2, 1): " << points.count({2, 1}) << ", (5, 5): " << points.count({5, 5}) << '\n';
    std::unordered_set<Point, BadHash> bad;
    for (int i = 0; i < 100; ++i) bad.insert({i, i});
    std::cout << "bad hash: the bucket of key (0, 0) holds " << bad.bucket_size(bad.bucket({0, 0}))
              << " of 100 elements -- lookup in linear\n";
}

// ---------------------------------------------------------------- 6
void section6() {
    std::cout << "\n== 6. C++17: extract and merge\n";
    // Ključ u mapi je const. Da se promeni bez nove alokacije: extract čvor,
    // promeni ključ, vrati.
    std::map<int, std::string> devices{{1, "sensor"}, {2, "motor"}};
    auto node = devices.extract(1);
    node.key() = 10;
    devices.insert(std::move(node));
    for (const auto& [addr, name] : devices) std::cout << ' ' << addr << ':' << name;
    std::cout << '\n';
    std::set<int> a{1, 3, 5}, b{2, 3, 4};
    a.merge(b);                                      // premesti čvorove iz b; duplikati ostaju u b
    print("a after merge", a);
    print("b after merge", b);
}

int main() {
    std::cout << std::boolalpha;
    section1();
    section2();
    section3();
    section4();
    section5();
    section6();
}
