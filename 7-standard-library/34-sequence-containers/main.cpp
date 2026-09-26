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
// ./check_cases.sh 7-standard-library/34-sequence-containers  proverava oba.

// Brojač alokacija: zamena globalnog operator new (kao u lekciji 31).
static int allocations = 0;
void* operator new(std::size_t n) {
    ++allocations;
    if (void* p = std::malloc(n ? n : 1)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

template <typename C>
void print(const char* label, const C& c) {
    std::cout << label << ':';
    for (const auto& x : c) std::cout << ' ' << x;
    std::cout << '\n';
}

// ---------------------------------------------------------------- 1
// Algoritam ne zna za kontejner -- radi nad opsegom iteratora [first, last).
template <typename It>
int sum(It first, It last) {
    int s = 0;
    for (; first != last; ++first) s += *first;
    return s;
}

void section1() {
    std::cout << "\n== 1. containers, iterators, algorithms\n";
    std::vector<int> v{1, 2, 3};
    std::list<int> l{4, 5, 6};
    int arr[] = {7, 8, 9};
    // Isti algoritam za tri potpuno različita "kontejnera".
    std::cout << "sum: vector " << sum(v.begin(), v.end()) << ", list " << sum(l.begin(), l.end())
              << ", C array " << sum(std::begin(arr), std::end(arr)) << '\n';
    // Iterator vektora je random access (it + 2), iterator liste samo
    // bidirectional (++, --) -- zato std::sort ne radi na listi (errors/e01).
    std::cout << "v.begin() + 2 -> " << *(v.begin() + 2) << ", std::next(l.begin(), 2) -> "
              << *std::next(l.begin(), 2) << '\n';
}

// ---------------------------------------------------------------- 2
void section2() {
    std::cout << "\n== 2. std::array: fixed size, no heap\n";
    int before = allocations;
    std::array<int, 5> a{5, 3, 1, 4, 2};
    std::sort(a.begin(), a.end());
    print("sorted", a);
    std::cout << "size " << a.size() << ", sizeof " << sizeof(a) << " (= 5 * sizeof(int)), allocations: "
              << allocations - before << '\n';
}

// ---------------------------------------------------------------- 3
void section3() {
    std::cout << "\n== 3. std::vector: one block, grows\n";
    int before = allocations;
    std::vector<int> without;
    for (int i = 0; i < 100; ++i) without.push_back(i);
    int afterWithout = allocations;
    std::vector<int> with;
    with.reserve(100);
    for (int i = 0; i < 100; ++i) with.push_back(i);
    int afterWith = allocations;
    std::cout << "100 x push_back: allocations without reserve " << afterWithout - before << ", with reserve "
              << afterWith - afterWithout << '\n';
    // Rast premesti elemente u novi blok: stari pokazivači vise (ub/ lekcije 04 i 07).
    std::vector<int> v{1, 2, 3};
    const int* oldData = v.data();
    v.push_back(4);                                 // kapacitet 3 -> novi blok
    std::cout << "after growth data() changed: " << (oldData != v.data()) << '\n';
    // Umetanje na početak pomera SVE elemente: O(n).
    v.insert(v.begin(), 0);
    print("insert at the front", v);
}

// ---------------------------------------------------------------- 4
void section4() {
    std::cout << "\n== 4. std::deque: fast at both ends\n";
    std::deque<int> d{2, 3};
    d.push_front(1);
    d.push_back(4);
    print("deque", d);
    std::cout << "d[2] = " << d[2] << " (random access)\n";
    // Reference na elemente ostaju važeće posle push_front/push_back (blokovi
    // se ne premeštaju) -- za razliku od vektora.
    int& first = d[0];
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
        d.push_front(i);
    }
    std::cout << "reference after 2000 insertions: " << first << ", size " << d.size() << '\n';
    int before = allocations;
    std::deque<int> hundred;
    for (int i = 0; i < 100; ++i) hundred.push_back(i);
    std::cout << "100 x push_back into deque: allocations " << allocations - before << " (blocks, not element by element)\n";
}

// ---------------------------------------------------------------- 5
void section5() {
    std::cout << "\n== 5. std::list and std::forward_list: linked lists\n";
    int before = allocations;
    std::list<int> l;
    for (int i = 0; i < 100; ++i) l.push_back(i);
    std::cout << "100 x push_back into list: allocations " << allocations - before << " (one per node)\n";

    std::list<int> a{1, 2, 3, 4};
    auto it = std::next(a.begin());                 // pokazuje na 2
    a.push_front(0);
    a.push_back(5);
    a.insert(it, 99);                               // O(1) na poziciji iteratora
    std::cout << "iterator after the insertion still points to " << *it << '\n';
    print("list", a);

    std::list<int> b{10, 20};
    a.splice(a.begin(), b);                         // premesti SVE čvorove iz b, O(1), bez kopija
    print("after splice", a);
    std::cout << "b.size() = " << b.size() << '\n';
    a.sort();                                       // lista ima svoj sort (std::sort ne radi)
    a.remove(99);
    print("sort + remove(99)", a);

    std::forward_list<int> f{3, 1, 2};
    f.push_front(0);
    f.insert_after(f.before_begin(), -1);           // "posle" -- jednostruka lista ide samo napred
    f.sort();
    print("forward_list", f);
    // forward_list nema size() (errors/e02): čuva samo pokazivač na početak.
    std::cout << "element count (distance): " << std::distance(f.begin(), f.end())
              << "; sizeof(forward_list) = " << sizeof(f) << ", sizeof(list) = " << sizeof(a) << '\n';
}

// ---------------------------------------------------------------- 6
void section6() {
    std::cout << "\n== 6. common interface\n";
    std::vector<int> v(5);
    std::iota(v.begin(), v.end(), 1);               // 1 2 3 4 5
    std::deque<int> d(v.begin(), v.end());          // svaki kontejner se pravi iz opsega
    std::list<int> l(v.rbegin(), v.rend());         // obrnutim redom
    print("deque from the vector", d);
    print("list from rbegin/rend", l);
    std::cout << "front/back: " << l.front() << '/' << l.back() << ", empty: " << l.empty() << '\n';
    // erase-remove radi za vector i deque; lista ima remove_if član (brži, bez pomeranja).
    v.erase(std::remove_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; }), v.end());
    l.remove_if([](int x) { return x % 2 == 0; });
    print("vector without evens", v);
    print("list without evens", l);
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
