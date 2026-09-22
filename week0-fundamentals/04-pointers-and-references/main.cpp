#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

// Pokazivači i reference -- ISPRAVNI slučajevi. Sve u ovom fajlu se
// kompajlira i radi bez ijedne ASan/UBSan prijave (g++ 13 i clang 18).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior (sanitizer ga hvata)
// ./check_cases.sh week0-fundamentals/04-pointers-and-references  proverava oba.
// Numeracija sekcija prati notes.md. Warning za sizeof na parametru-nizu
// (sekcija 3) je namerni.

// ---------------------------------------------------------------- 1
void s01_pointerBasics() {
    std::cout << "-- 1. pokazivač: osnove --\n";
    int x = 5;
    int* p = &x; // & -- adresa od x
    *p = 7;      // * -- pristup objektu na toj adresi
    std::cout << "x=" << x << " (promenjen kroz *p)\n";

    // Pokazivač je i sam OBJEKAT: ima svoju adresu i veličinu, i može da
    // se preusmeri.
    int y = 9;
    p = &y;
    std::cout << "*p posle p = &y: " << *p << "; sizeof(p)=" << sizeof(p)
              << "; &p != &x: " << std::boolalpha << (static_cast<void*>(&p) != static_cast<void*>(&x))
              << std::noboolalpha << "\n";

    // Zamka u deklaraciji: * pripada IMENU, ne tipu.
    int* a1, b1;   // a1 je int*, b1 je OBIČAN int
    int *a2, *b2;  // oba pokazivača
    static_assert(std::is_same_v<decltype(a1), int*>);
    static_assert(std::is_same_v<decltype(b1), int>);
    static_assert(std::is_same_v<decltype(b2), int*>);
    a1 = &x; b1 = 0; a2 = &x; b2 = &y;
    std::cout << "int* a1, b1; -> b1 je int (static_assert)\n";
    (void)a1; (void)b1; (void)a2; (void)b2;
}

// ---------------------------------------------------------------- 2
void f(int) { std::cout << "  -> f(int)\n"; }
void f(char*) { std::cout << "  -> f(char*)\n"; }

template <typename F, typename P>
void call(F func, P param) {
    func(param);
}
void g(int* p) { std::cout << "  -> g(int*) dobio " << (p ? "ne-null" : "nullptr") << "\n"; }

void s02_nullptr() {
    std::cout << "-- 2. null pokazivač i nullptr (EMC Item 8) --\n";
    int* p = nullptr;
    int* q{}; // value-init pokazivača -> nullptr (lekcija 03)
    if (!p && q == nullptr) std::cout << "p i q su null; uvek proveri pre dereferenciranja\n";

    std::cout << "f(0):\n";
    f(0);       // 0 je int -> f(int)
    std::cout << "f(nullptr):\n";
    f(nullptr); // nullptr ide SAMO u pokazivače -> f(char*); f(NULL) je dvosmislen (errors/e07)

    std::cout << "call(g, nullptr):\n";
    call(g, nullptr); // std::nullptr_t -> int* radi i kroz template; call(g, 0) ne (errors/e08)
    static_assert(std::is_same_v<decltype(nullptr), std::nullptr_t>);
}

// ---------------------------------------------------------------- 3
void decayed(int arr[]) { // arr je ovde int* -- niz se "raspao" u pokazivač
    std::cout << "  sizeof(arr) u funkciji = " << sizeof(arr) << " (veličina POKAZIVAČA)\n";
}

template <std::size_t N>
std::size_t lengthOf(int (&)[N]) { // referenca na niz čuva veličinu u tipu
    return N;
}

void s03_arithmeticAndArrays() {
    std::cout << "-- 3. pointer arithmetic i nizovi --\n";
    int arr[5] = {10, 20, 30, 40, 50};
    int* p = arr; // niz -> pokazivač na prvi element (array-to-pointer decay)
    std::cout << "*p=" << *p << " *(p+1)=" << *(p + 1) << " p[2]=" << p[2] << " (p[i] == *(p+i))\n";
    std::cout << "bajtova između p+1 i p: "
              << reinterpret_cast<char*>(p + 1) - reinterpret_cast<char*>(p)
              << " (= sizeof(int), korak zavisi od tipa)\n";

    std::ptrdiff_t n = (arr + 5) - arr; // razlika u ELEMENTIMA, samo unutar istog niza
    std::cout << "(arr + 5) - arr = " << n << "\n";

    // Pokazivač "jedan iza kraja" sme da se napravi i poredi -- ne i
    // dereferencira (ub/u02). Tako rade i end() iteratori.
    std::cout << "iteracija do arr + 5: ";
    for (int* it = arr; it != arr + 5; ++it) std::cout << *it << ' ';
    std::cout << "\n";

    std::cout << "sizeof(arr) u main = " << sizeof(arr) << " (ceo niz)\n";
    decayed(arr);
    std::cout << "std::size(arr)=" << std::size(arr) << " lengthOf(arr)=" << lengthOf(arr) << "\n";

    // Poređenje < pokazivača iz RAZLIČITIH objekata ima nespecificiran
    // rezultat; std::less garantuje dosledan (totalni) poredak.
    int a = 1, b = 2;
    bool ordered = std::less<int*>{}(&a, &b) || std::less<int*>{}(&b, &a);
    std::cout << "std::less daje poredak za nepovezane pokazivače: " << std::boolalpha << ordered
              << std::noboolalpha << "\n";
}

// ---------------------------------------------------------------- 4
void s04_voidPointer() {
    std::cout << "-- 4. void* --\n";
    int x = 42;
    void* vp = &x;                      // bilo koji T* -> void* je implicitno
    int* ip = static_cast<int*>(vp);    // nazad mora eksplicitno (errors/e03)
    std::cout << "*static_cast<int*>(vp) = " << *ip << "\n";
    // *vp i vp + 1 ne rade (errors/e04, e05): void nema tip ni veličinu.
}

// ---------------------------------------------------------------- 5
void allocateOld(int** out) { *out = new int(1); } // C stil: pokazivač na pokazivač
void allocateRef(int*& out) { out = new int(2); }  // C++: referenca na pokazivač

std::unique_ptr<int> allocateModern() { return std::make_unique<int>(3); } // najbolje: vrati vlasnika

void s05_pointerToPointer() {
    std::cout << "-- 5. pokazivač na pokazivač --\n";
    int x = 5;
    int* p = &x;
    int** pp = &p;
    **pp = 6;
    std::cout << "**pp = 6 -> x=" << x << "\n";

    // Primena: funkcija koja menja POZIVAOČEV pokazivač.
    int* a = nullptr;
    int* b = nullptr;
    allocateOld(&a);
    allocateRef(b);
    auto c = allocateModern();
    std::cout << "*a=" << *a << " *b=" << *b << " *c=" << *c << "\n";
    delete a;
    delete b; // c se oslobađa sam
}

// ---------------------------------------------------------------- 6
void s06_constAndPointers() {
    std::cout << "-- 6. const i pokazivači (čitaj s desna na levo) --\n";
    int x = 1, y = 2;

    const int* p1 = &x;       // pokazivač na const int: *p1 = ... ne (errors/e09)
    p1 = &y;                  // preusmeravanje: da
    int* const p2 = &x;       // const pokazivač na int: p2 = &y ne (errors/e10)
    *p2 = 10;                 // menjanje vrednosti: da
    const int* const p3 = &x; // ni jedno ni drugo
    x = 11;                   // x sam nije const -- p1/p3 samo zabranjuju izmenu KROZ njih

    const int c = 5;
    const int* pc = &c;       // na const objekat mora const int* (errors/e11)
    std::cout << "*p1=" << *p1 << " *p2=" << *p2 << " *p3=" << *p3 << " *pc=" << *pc << "\n";
    // Detaljnije: lekcija 07.
}

// ---------------------------------------------------------------- 7
void s07_referenceBasics() {
    std::cout << "-- 7. reference: osnove --\n";
    int x = 10;
    int& r = x; // MORA odmah da se veže (errors/e12)
    r = 20;     // piše u x
    std::cout << "x=" << x << "; &r == &x: " << std::boolalpha << (&r == &x)
              << "; sizeof(r) == sizeof(x): " << (sizeof(r) == sizeof(x)) << std::noboolalpha << "\n";

    int y = 99;
    r = y;      // NIJE preusmeravanje: kopira 99 u x, r i dalje "je" x
    y = 0;
    std::cout << "posle r = y; y = 0;  x=" << x << " (referenca se ne preusmerava)\n";

    // Referenca nije objekat: nema niza referenci, pokazivača na referencu
    // ni reference na referencu (errors/e13, e14, e15). Kroz alias/template
    // važi "reference collapsing": int& & -> int&.
    using R = int&;
    R& rr = x;
    static_assert(std::is_same_v<decltype(rr), int&>);
    int* px = &x;
    int*& rpx = px; // referenca NA pokazivač postoji
    std::cout << "R& rr -> int& (static_assert); *rpx=" << *rpx << "\n";
}

// ---------------------------------------------------------------- 8
void s08_bindingAndLifetime() {
    std::cout << "-- 8. vezivanje referenci i produženje životnog veka --\n";
    const int& r1 = 5; // const& se veže za privremeni, a privremeni živi koliko i r1
    const std::string& s1 = std::string("privremeni string");
    std::string&& s2 = std::string("rvalue referenca");
    s2 += " (izmenjiva)";
    std::cout << "r1=" << r1 << " s1=" << s1 << " s2=" << s2 << "\n";
    // int& r = 5; ne radi (errors/e16); int&& r = x; ne radi (errors/e17).

    // Iznenađenje: const int& na double se veže za privremenu KOPIJU.
    double d = 1.5;
    const int& ri = d; // ri je vezan za privremeni int(1), ne za d
    d = 2.5;
    std::cout << "double d = 1.5; const int& ri = d; d = 2.5; -> ri=" << ri << " (ne prati d)\n";
    // int& ri = d; ne radi uopšte (errors/e19).

    // Produženje NE prolazi kroz funkciju: const int& r = std::max(1, 2);
    // visi (ub/u05). Kopija je bezbedna:
    int m = std::max(1, 2);
    std::cout << "int m = std::max(1, 2) -> " << m << "\n";
}

// ---------------------------------------------------------------- 9
// Referenca vs pokazivač: samo tabela u notes.md (slicing kod polimorfizma: lekcija 13).

// ---------------------------------------------------------------- 10
struct CopyCounter {
    CopyCounter() = default;
    CopyCounter(const CopyCounter&) { ++copies; }
    static inline int copies = 0;
};

void byValue(CopyCounter) {}
void byConstRef(const CopyCounter&) {}
void increment(int& v) { ++v; }                // izlazni parametar -- mora da postoji
void maybeIncrement(int* v) { if (v) ++*v; }   // opcioni parametar -- može nullptr

void s10_parameters() {
    std::cout << "-- 10. prosleđivanje parametara (EC++ Item 20) --\n";
    CopyCounter c;
    CopyCounter::copies = 0;
    byValue(c);
    std::cout << "byValue: kopija=" << CopyCounter::copies;
    CopyCounter::copies = 0;
    byConstRef(c);
    std::cout << "  byConstRef: kopija=" << CopyCounter::copies << "\n";
    byConstRef(CopyCounter{}); // const& prima i privremeni objekat

    int n = 1;
    increment(n);          // increment(5) ne radi (errors/e18)
    maybeIncrement(&n);
    maybeIncrement(nullptr);
    std::cout << "n posle increment + maybeIncrement = " << n << "\n";
}

// ---------------------------------------------------------------- 11
class Counter {
public:
    Counter& add(int v) { value_ += v; return *this; } // *this: živi duže od poziva
    const int& value() const { return value_; }        // referenca na ČLAN
    static int& instances() { static int count = 0; return count; } // static živi do kraja programa

private:
    int value_ = 0;
};

void s11_returningReferences() {
    std::cout << "-- 11. vraćanje referenci (EC++ Item 21) --\n";
    Counter c;
    c.add(1).add(2).add(3);           // chaining kroz vraćeni *this
    const int& v = c.value();         // OK: c živi duže od v
    ++Counter::instances();
    std::cout << "c.value()=" << v << " instances=" << Counter::instances() << "\n";
    // Vraćanje adrese/reference LOKALNE promenljive -- ub/u03, ub/u04.
    // Referenca na član PRIVREMENOG objekta -- ub/u11.
}

// ---------------------------------------------------------------- 12
void s12_invalidation() {
    std::cout << "-- 12. pokazivači na elemente kontejnera --\n";
    std::vector<int> v{1, 2, 3};
    std::size_t index = 0;   // indeks preživljava realokaciju
    for (int i = 0; i < 100; ++i) v.push_back(i);
    int* first = &v[index];  // pokazivač uzet POSLE poslednje izmene veličine
    std::cout << "v[index]=" << v[index] << " *first=" << *first << "\n";

    std::vector<int> w;
    w.reserve(200);          // kapacitet unapred -> nema realokacije do 200
    w.push_back(1);
    int* stable = &w[0];
    for (int i = 0; i < 100; ++i) w.push_back(i);
    std::cout << "posle reserve, *stable=" << *stable << " (i dalje validan)\n";
    // Bez toga pokazivač visi -- ub/u06.
}

// ---------------------------------------------------------------- 13
void s13_ownership() {
    std::cout << "-- 13. vlasništvo: new/delete i pametni pokazivači --\n";
    int* one = new int(1);
    int* many = new int[3]{1, 2, 3};
    delete one;     // new   -> delete
    delete[] many;  // new[] -> delete[]  (EC++ Item 16; mešanje -- ub/u09)

    auto owned = std::make_unique<int>(42);        // oslobađa se sam
    auto buffer = std::make_unique<int[]>(3);
    int* observer = owned.get();                   // sirov pokazivač = NE-vlasnik
    std::cout << "*owned=" << *owned << " *observer=" << *observer << " buffer[0]=" << buffer[0] << "\n";
    // use-after-free, double free, curenje: ub/u07, u08, u10.
}

// ---------------------------------------------------------------- 14
struct Point {
    int x = 0;
    int y = 0;
    void shift(int d) { x += d; y += d; }
};

void s14_pointerToMember() {
    std::cout << "-- 14. pokazivač na člana klase --\n";
    int Point::* coord = &Point::x;          // "koji član", bez objekta
    void (Point::* shiftFn)(int) = &Point::shift;

    Point p;
    p.*coord = 5;          // član x OBJEKTA p
    coord = &Point::y;
    p.*coord = 7;
    (p.*shiftFn)(1);
    Point* pp = &p;
    (pp->*shiftFn)(1);
    std::cout << "p = (" << p.x << ", " << p.y << ")\n";
}

int main() {
    s01_pointerBasics();
    s02_nullptr();
    s03_arithmeticAndArrays();
    s04_voidPointer();
    s05_pointerToPointer();
    s06_constAndPointers();
    s07_referenceBasics();
    s08_bindingAndLifetime();
    s10_parameters();
    s11_returningReferences();
    s12_invalidation();
    s13_ownership();
    s14_pointerToMember();
}
