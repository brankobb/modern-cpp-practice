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
// ./check_cases.sh week3-advanced/s17-associative-containers  proverava oba.

template <typename C>
void ispisi(const char* opis, const C& c) {
    std::cout << opis << ':';
    for (const auto& x : c) std::cout << ' ' << x;
    std::cout << '\n';
}

// ---------------------------------------------------------------- 1
void sekcija1() {
    std::cout << "\n== 1. std::set i std::multiset\n";
    std::set<int> s{5, 1, 4, 1, 3};                 // sortiran, bez duplikata
    ispisi("set", s);
    auto [it, ubaceno] = s.insert(4);               // već postoji
    std::cout << "insert(4): ubačeno " << ubaceno << ", iterator na " << *it << '\n';
    std::cout << "count(3) " << s.count(3) << ", find(7) == end: " << (s.find(7) == s.end()) << '\n';
    std::cout << "lower_bound(2) " << *s.lower_bound(2) << ", upper_bound(4) " << *s.upper_bound(4)
              << '\n';

    std::multiset<int> ms{1, 2, 2, 2, 3};           // sortiran, SA duplikatima
    std::multiset<int> ms2 = ms;
    auto obrisano = ms.erase(2);                    // briše SVE jednake
    ms2.erase(ms2.find(2));                         // briše JEDAN
    std::cout << "multiset.erase(2) obrisao " << obrisano << '\n';
    ispisi("ostalo", ms);
    ispisi("multiset.erase(find(2))", ms2);
}

// ---------------------------------------------------------------- 2
struct PoDuzini {   // poredi po dužini -- "jednaki" su stringovi iste dužine
    bool operator()(const std::string& a, const std::string& b) const { return a.size() < b.size(); }
};

void sekcija2() {
    std::cout << "\n== 2. poredak i ekvivalencija\n";
    std::set<int, std::greater<int>> opadajuce{1, 3, 2};
    ispisi("set<int, greater>", opadajuce);
    // Poredak određuje i šta je "isto": !(a < b) && !(b < a).
    std::set<std::string, PoDuzini> poDuzini{"aa", "b", "cc", "ddd"};
    ispisi("set po dužini (\"cc\" je \"isto\" što i \"aa\")", poDuzini);
    // Lambda kao poredak: tip je decltype(lambda), a objekat se prosledi
    // konstruktoru.
    auto bezVelikih = [](const std::string& a, const std::string& b) {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(),
                                            [](unsigned char x, unsigned char y) { return std::tolower(x) < std::tolower(y); });
    };
    std::set<std::string, decltype(bezVelikih)> imena(bezVelikih);
    for (const char* i : {"Ana", "bora", "ANA", "Cane"}) imena.insert(i);
    ispisi("bez obzira na velika slova", imena);
}

// ---------------------------------------------------------------- 3
void sekcija3() {
    std::cout << "\n== 3. std::map i std::multimap\n";
    std::map<std::string, int> kanali{{"temp", 1}, {"pritisak", 2}};
    kanali["vlaga"] = 3;                             // [] ubaci ako ne postoji
    std::cout << "kanali[\"nepoznat\"] = " << kanali["nepoznat"] << " -- i sada size = " << kanali.size()
              << '\n';
    // Čitanje bez ubacivanja: find ili at.
    if (auto it = kanali.find("temp"); it != kanali.end()) std::cout << "find temp: " << it->second << '\n';
    try {
        std::cout << kanali.at("nema");
    } catch (const std::out_of_range&) {
        std::cout << "at(\"nema\"): out_of_range\n";
    }
    // insert NE prepisuje postojeći; insert_or_assign i try_emplace (C++17).
    auto [it, ubaceno] = kanali.insert({"temp", 99});
    std::cout << "insert postojećeg: ubačeno " << ubaceno << ", vrednost ostala " << it->second << '\n';
    kanali.insert_or_assign("temp", 10);
    kanali.try_emplace("temp", 77);                  // postoji: ne radi ništa (ni ne pravi vrednost)
    kanali.try_emplace("struja", 4);
    for (const auto& [ime, kanal] : kanali) std::cout << ' ' << ime << '=' << kanal;
    std::cout << '\n';

    std::multimap<std::string, int> merenja{{"temp", 21}, {"pritisak", 1013}, {"temp", 22}, {"temp", 20}};
    auto [od, doKraja] = merenja.equal_range("temp");   // svi sa ključem "temp", redom ubacivanja
    std::cout << "temp merenja:";
    for (auto i = od; i != doKraja; ++i) std::cout << ' ' << i->second;
    std::cout << " (ukupno ključeva temp: " << merenja.count("temp") << ")\n";
}

// ---------------------------------------------------------------- 4
void sekcija4() {
    std::cout << "\n== 4. neuređeni kontejneri: heš tabela\n";
    std::unordered_map<std::string, int> brojac;
    for (const char* rec : {"a", "b", "a", "c", "a", "b"}) ++brojac[rec];
    // Redosled iteracije NIJE određen -- za ispis ga sortiramo.
    std::vector<std::pair<std::string, int>> sortirano(brojac.begin(), brojac.end());
    std::sort(sortirano.begin(), sortirano.end());
    for (const auto& [rec, n] : sortirano) std::cout << ' ' << rec << '=' << n;
    std::cout << '\n';
    std::cout << "size " << brojac.size() << ", bucket_count " << brojac.bucket_count() << ", load_factor "
              << brojac.load_factor() << '\n';
    std::unordered_set<int> u;
    u.reserve(100);                                  // bucket-i za 100 elemenata unapred: bez rehash-a
    std::size_t pre = u.bucket_count();
    for (int i = 0; i < 100; ++i) u.insert(i);
    std::cout << "posle reserve(100) i 100 insert-a bucket_count isti: " << (pre == u.bucket_count())
              << '\n';
}

// ---------------------------------------------------------------- 5
struct Tacka {
    int x, y;
    bool operator==(const Tacka& o) const { return x == o.x && y == o.y; }
};

// Heš za sopstveni tip: funkcijski objekat koji kombinuje heševe polja.
struct HesTacke {
    std::size_t operator()(const Tacka& t) const noexcept {
        std::size_t h = std::hash<int>{}(t.x);
        return h ^ (std::hash<int>{}(t.y) + 0x9e3779b9 + (h << 6) + (h >> 2));
    }
};

// Loš heš: svi ključevi u isti bucket.
struct LosHes {
    std::size_t operator()(const Tacka&) const noexcept { return 42; }
};

void sekcija5() {
    std::cout << "\n== 5. std::hash i sopstveni heš\n";
    std::cout << "std::hash<int>{}(42) == std::hash<int>{}(42): "
              << (std::hash<int>{}(42) == std::hash<int>{}(42)) << '\n';
    std::unordered_set<Tacka, HesTacke> tacke{{1, 2}, {2, 1}, {3, 4}};
    std::cout << "sadrži (2, 1): " << tacke.count({2, 1}) << ", (5, 5): " << tacke.count({5, 5}) << '\n';
    std::unordered_set<Tacka, LosHes> los;
    for (int i = 0; i < 100; ++i) los.insert({i, i});
    std::cout << "loš heš: u bucket-u ključa (0, 0) je " << los.bucket_size(los.bucket({0, 0}))
              << " od 100 elemenata -- traženje je linearno\n";
}

// ---------------------------------------------------------------- 6
void sekcija6() {
    std::cout << "\n== 6. C++17: extract i merge\n";
    // Ključ u mapi je const. Da se promeni bez nove alokacije: extract čvor,
    // promeni ključ, vrati.
    std::map<int, std::string> uredjaji{{1, "senzor"}, {2, "motor"}};
    auto cvor = uredjaji.extract(1);
    cvor.key() = 10;
    uredjaji.insert(std::move(cvor));
    for (const auto& [adr, ime] : uredjaji) std::cout << ' ' << adr << ':' << ime;
    std::cout << '\n';
    std::set<int> a{1, 3, 5}, b{2, 3, 4};
    a.merge(b);                                      // premesti čvorove iz b; duplikati ostaju u b
    ispisi("a posle merge", a);
    ispisi("b posle merge", b);
}

int main() {
    std::cout << std::boolalpha;
    sekcija1();
    sekcija2();
    sekcija3();
    sekcija4();
    sekcija5();
    sekcija6();
}
