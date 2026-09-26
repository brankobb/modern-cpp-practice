#include <algorithm>
#include <cstdio>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

// std::vector i std::initializer_list -- ISPRAVNI slučajevi. Sve se
// kompajlira i radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 1-language-basics/07-vector-and-initializer-list  proverava oba.
// Vektor se pojavljuje i u drugim lekcijama; ovde je na jednom mestu, sa
// uputima: 2D (13), invalidacija (04, lekcija 27), realokacija i noexcept
// (lekcije 23, 25), vector<bool> (10), push_back vs emplace (lekcija 24).

template <typename T>
void print(const char* label, const std::vector<T>& v) {
    std::cout << "  " << label << " [";
    for (std::size_t i = 0; i < v.size(); ++i) std::cout << (i ? " " : "") << v[i];
    std::cout << "] size=" << v.size() << "\n";
}

// ---------------------------------------------------------------- 1
void s01_construction() {
    std::cout << "-- 1. construction --\n";
    std::vector<int> zeros(3);          // 3 elementa, value-init -> 0
    std::vector<int> sevens(3, 7);      // 3 elementa, svaki 7
    std::vector<int> list{3, 7};        // initializer_list: 2 elementa (lekcija 03, EMC Item 7)
    int raw[] = {5, 6, 7, 8};
    std::vector<int> fromRange(std::begin(raw) + 1, std::end(raw)); // iz opsega iteratora
    print("vector<int>(3)    ", zeros);
    print("vector<int>(3, 7) ", sevens);
    print("vector<int>{3, 7} ", list);
    print("from range raw+1..", fromRange);
}

// ---------------------------------------------------------------- 2
void s02_sizeAndCapacity() {
    std::cout << "-- 2. size vs capacity --\n";
    std::vector<int> v;
    std::cout << "  capacity after push_back: ";
    std::size_t last = v.capacity();
    for (int i = 0; i < 17; ++i) {
        v.push_back(i);
        if (v.capacity() != last) {
            last = v.capacity();
            std::cout << last << " ";
        }
    }
    std::cout << " <- libstdc++ doubles it; every change = reallocation + moving\n";
    v.clear();
    std::cout << "  after clear(): size=" << v.size() << " capacity=" << v.capacity() << " (memory stays)\n";
    v.shrink_to_fit(); // zahtev, ne obaveza
    std::cout << "  after shrink_to_fit(): capacity=" << v.capacity() << "\n";
    std::vector<int> planned;
    planned.reserve(100); // jedna alokacija unapred
    for (int i = 0; i < 100; ++i) planned.push_back(i);
    std::cout << "  reserve(100) then 100 x push_back: capacity=" << planned.capacity() << " (no reallocation)\n";
}

// ---------------------------------------------------------------- 3
struct Sensor {
    explicit Sensor(int sensorId = 0) : id(sensorId) {}
    int id;
};

void s03_reserveVsResize() {
    std::cout << "-- 3. reserve vs resize --\n";
    std::vector<int> a;
    a.reserve(5);  // SAMO kapacitet: size ostaje 0, elemenata nema (a[0] je UB, ub/u01)
    std::vector<int> b;
    b.resize(5);   // pravi 5 elemenata (0)
    std::vector<int> c;
    c.resize(3, 9); // 3 elementa sa vrednošću 9
    std::cout << "  reserve(5): size=" << a.size() << " capacity>=" << (a.capacity() >= 5 ? "5" : "?") << "\n";
    print("resize(5):  ", b);
    print("resize(3,9):", c);
    std::vector<Sensor> sensors;
    sensors.resize(2); // traži podrazumevani konstruktor (bez njega: errors/e04)
    std::cout << "  vector<Sensor>.resize(2): id=" << sensors[0].id << "," << sensors[1].id << "\n";
}

// ---------------------------------------------------------------- 4
void s04_access() {
    std::cout << "-- 4. access --\n";
    std::vector<int> v{10, 20, 30};
    std::cout << "  v[1]=" << v[1] << " v.at(1)=" << v.at(1) << " front=" << v.front() << " back=" << v.back();
    try {
        (void)v.at(10);
    } catch (const std::out_of_range&) {
        std::cout << "; v.at(10) -> std::out_of_range";
    }
    std::cout << "\n  front()/back() on an empty vector: UB (ub/u02); v[10]: UB without a check\n";
    std::printf("  data() for a C API: %d %d %d\n", v.data()[0], v.data()[1], v.data()[2]); // uzastopna memorija
}

// ---------------------------------------------------------------- 5
void s05_modify() {
    std::cout << "-- 5. modifications --\n";
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    v.insert(v.begin() + 1, 99);                   // pomera sve iza: O(n)
    v.erase(v.begin());                            // pomera sve iza: O(n)
    v.pop_back();                                  // O(1)
    print("insert(+1, 99), erase(begin), pop_back:", v);
    v.erase(std::remove_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; }), v.end()); // erase-remove
    print("after removing even numbers:            ", v);
    std::vector<std::string> names;
    names.emplace_back(3, 'a');                    // konstruiše std::string(3, 'a') direktno u vektoru
    names.push_back("bob");
    std::cout << "  emplace_back(3, 'a') -> \"" << names[0] << "\", push_back(\"bob\") -> \"" << names[1] << "\"\n";
    std::cout << "  (iterator invalidation after insert/erase: lesson 27)\n";
}

// ---------------------------------------------------------------- 6
struct Tracked {
    Tracked() = default;
    Tracked(const Tracked&) { ++copies; }
    Tracked(Tracked&&) noexcept { ++moves; }
    inline static int copies = 0;
    inline static int moves = 0;
};

int sum(std::initializer_list<int> values) { // funkcija koja prima listu proizvoljne dužine
    return std::accumulate(values.begin(), values.end(), 0);
}

class Polygon {
public:
    Polygon(std::initializer_list<int> sides) : sides_(sides) {} // KOPIRA elemente u vektor
    std::size_t count() const { return sides_.size(); }

private:
    std::vector<int> sides_; // ne čuvaj initializer_list kao član (ub/u03)
};

void s06_initializerList() {
    std::cout << "-- 6. std::initializer_list (course 92) --\n";
    std::cout << "  sum({1, 2, 3, 4})=" << sum({1, 2, 3, 4}) << ", sum({})=" << sum({}) << "\n";
    Polygon triangle{3, 4, 5};
    std::cout << "  Polygon{3, 4, 5}.count()=" << triangle.count() << "\n";

    Tracked::copies = Tracked::moves = 0;
    std::vector<Tracked> fromList{Tracked(), Tracked(), Tracked()}; // elementi liste su const -> KOPIJE
    int listCopies = Tracked::copies;
    Tracked::copies = Tracked::moves = 0;
    std::vector<Tracked> built;
    built.reserve(3);
    for (int i = 0; i < 3; ++i) built.emplace_back();
    std::cout << "  vector<Tracked>{t, t, t}: copies=" << listCopies << "; reserve + 3 x emplace_back: copies=" << Tracked::copies
              << " move=" << Tracked::moves << "\n";
    std::cout << "  <- you cannot move out of an initializer_list; that is why vector<unique_ptr<T>>{...} fails (errors/e01)\n";

    std::vector<std::unique_ptr<int>> owners;
    owners.push_back(std::make_unique<int>(1)); // move-only tipovi: push_back / emplace_back
    owners.push_back(std::make_unique<int>(2));
    std::cout << "  vector<unique_ptr<int>> via push_back: " << *owners[0] << " " << *owners[1] << "\n";
}

int main() {
    s01_construction();
    s02_sizeAndCapacity();
    s03_reserveVsResize();
    s04_access();
    s05_modify();
    s06_initializerList();
}
