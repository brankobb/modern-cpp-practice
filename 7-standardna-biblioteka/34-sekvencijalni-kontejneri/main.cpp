#include <algorithm>
#include <array>
#include <cstdlib>
#include <deque>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <list>
#include <new>
#include <numeric>
#include <vector>

// Sekvencijalni kontejneri -- ISPRAVNI slučajevi. Sve se kompajlira bez
// upozorenja i radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i
// C++20). Brojevi sekcija prate notes.md. Brojevi alokacija i sizeof su za
// libstdc++ (g++ i clang na Linux-u); libc++ može da da druge.
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 7-standardna-biblioteka/34-sekvencijalni-kontejneri  proverava oba.

// Brojač alokacija: zamena globalnog operator new (kao u lekciji 31).
static int alokacija = 0;
void* operator new(std::size_t n) {
    ++alokacija;
    if (void* p = std::malloc(n ? n : 1)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

template <typename C>
void ispisi(const char* opis, const C& c) {
    std::cout << opis << ':';
    for (const auto& x : c) std::cout << ' ' << x;
    std::cout << '\n';
}

// ---------------------------------------------------------------- 1
// Algoritam ne zna za kontejner -- radi nad opsegom iteratora [first, last).
template <typename It>
int zbir(It first, It last) {
    int s = 0;
    for (; first != last; ++first) s += *first;
    return s;
}

void sekcija1() {
    std::cout << "\n== 1. kontejneri, iteratori, algoritmi\n";
    std::vector<int> v{1, 2, 3};
    std::list<int> l{4, 5, 6};
    int niz[] = {7, 8, 9};
    // Isti algoritam za tri potpuno različita "kontejnera".
    std::cout << "zbir: vector " << zbir(v.begin(), v.end()) << ", list " << zbir(l.begin(), l.end())
              << ", C niz " << zbir(std::begin(niz), std::end(niz)) << '\n';
    // Iterator vektora je random access (it + 2), iterator liste samo
    // bidirectional (++, --) -- zato std::sort ne radi na listi (errors/e01).
    std::cout << "v.begin() + 2 -> " << *(v.begin() + 2) << ", std::next(l.begin(), 2) -> "
              << *std::next(l.begin(), 2) << '\n';
}

// ---------------------------------------------------------------- 2
void sekcija2() {
    std::cout << "\n== 2. std::array: fiksna veličina, bez heap-a\n";
    int pre = alokacija;
    std::array<int, 5> a{5, 3, 1, 4, 2};
    std::sort(a.begin(), a.end());
    ispisi("sortiran", a);
    std::cout << "size " << a.size() << ", sizeof " << sizeof(a) << " (= 5 * sizeof(int)), alokacija: "
              << alokacija - pre << '\n';
}

// ---------------------------------------------------------------- 3
void sekcija3() {
    std::cout << "\n== 3. std::vector: jedan blok, raste\n";
    int pre = alokacija;
    std::vector<int> bez;
    for (int i = 0; i < 100; ++i) bez.push_back(i);
    int posleBez = alokacija;
    std::vector<int> sa;
    sa.reserve(100);
    for (int i = 0; i < 100; ++i) sa.push_back(i);
    int posleSa = alokacija;
    std::cout << "100 x push_back: alokacija bez reserve " << posleBez - pre << ", sa reserve "
              << posleSa - posleBez << '\n';
    // Rast premesti elemente u novi blok: stari pokazivači vise (ub/ lekcije 04 i 07).
    std::vector<int> v{1, 2, 3};
    const int* staro = v.data();
    v.push_back(4);                                 // kapacitet 3 -> novi blok
    std::cout << "posle rasta data() promenjen: " << (staro != v.data()) << '\n';
    // Umetanje na početak pomera SVE elemente: O(n).
    v.insert(v.begin(), 0);
    ispisi("insert na početak", v);
}

// ---------------------------------------------------------------- 4
void sekcija4() {
    std::cout << "\n== 4. std::deque: brz na oba kraja\n";
    std::deque<int> d{2, 3};
    d.push_front(1);
    d.push_back(4);
    ispisi("deque", d);
    std::cout << "d[2] = " << d[2] << " (random access)\n";
    // Reference na elemente ostaju važeće posle push_front/push_back (blokovi
    // se ne premeštaju) -- za razliku od vektora.
    int& prvi = d[0];
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
        d.push_front(i);
    }
    std::cout << "referenca posle 2000 dodavanja: " << prvi << ", size " << d.size() << '\n';
    int pre = alokacija;
    std::deque<int> sto;
    for (int i = 0; i < 100; ++i) sto.push_back(i);
    std::cout << "100 x push_back u deque: alokacija " << alokacija - pre << " (blokovi, ne element po element)\n";
}

// ---------------------------------------------------------------- 5
void sekcija5() {
    std::cout << "\n== 5. std::list i std::forward_list: povezane liste\n";
    int pre = alokacija;
    std::list<int> l;
    for (int i = 0; i < 100; ++i) l.push_back(i);
    std::cout << "100 x push_back u list: alokacija " << alokacija - pre << " (po jedna za svaki čvor)\n";

    std::list<int> a{1, 2, 3, 4};
    auto it = std::next(a.begin());                 // pokazuje na 2
    a.push_front(0);
    a.push_back(5);
    a.insert(it, 99);                               // O(1) na poziciji iteratora
    std::cout << "iterator posle umetanja i dalje pokazuje na " << *it << '\n';
    ispisi("list", a);

    std::list<int> b{10, 20};
    a.splice(a.begin(), b);                         // premesti SVE čvorove iz b, O(1), bez kopija
    ispisi("posle splice", a);
    std::cout << "b.size() = " << b.size() << '\n';
    a.sort();                                       // lista ima svoj sort (std::sort ne radi)
    a.remove(99);
    ispisi("sort + remove(99)", a);

    std::forward_list<int> f{3, 1, 2};
    f.push_front(0);
    f.insert_after(f.before_begin(), -1);           // "posle" -- jednostruka lista ide samo napred
    f.sort();
    ispisi("forward_list", f);
    // forward_list nema size() (errors/e02): čuva samo pokazivač na početak.
    std::cout << "broj elemenata (distance): " << std::distance(f.begin(), f.end())
              << "; sizeof(forward_list) = " << sizeof(f) << ", sizeof(list) = " << sizeof(a) << '\n';
}

// ---------------------------------------------------------------- 6
void sekcija6() {
    std::cout << "\n== 6. zajednički interfejs\n";
    std::vector<int> v(5);
    std::iota(v.begin(), v.end(), 1);               // 1 2 3 4 5
    std::deque<int> d(v.begin(), v.end());          // svaki kontejner se pravi iz opsega
    std::list<int> l(v.rbegin(), v.rend());         // obrnutim redom
    ispisi("deque iz vektora", d);
    ispisi("list iz rbegin/rend", l);
    std::cout << "front/back: " << l.front() << '/' << l.back() << ", empty: " << l.empty() << '\n';
    // erase-remove radi za vector i deque; lista ima remove_if član (brži, bez pomeranja).
    v.erase(std::remove_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; }), v.end());
    l.remove_if([](int x) { return x % 2 == 0; });
    ispisi("vector bez parnih", v);
    ispisi("list bez parnih", l);
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
