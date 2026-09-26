#include <array>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <new>
#include <string>
#include <utility>
#include <vector>

// std::function i std::bind -- ISPRAVNI slučajevi. Sve se kompajlira bez
// upozorenja i radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i
// C++20). Brojevi sekcija prate notes.md.
// POGREŠNI slučajevi:
//   errors/   -- kod koji se NE kompajlira
//   ub/       -- kod koji se kompajlira, ali je undefined behavior
//   runtime/  -- kod koji se kompajlira, a program se prekine
// ./check_cases.sh 5-functions-as-values/31-function-and-bind  proverava sve.

// Brojač alokacija (za sekciju 2): zamena globalnog operator new.
// Standard to dozvoljava; ovde služi samo da se VIDI kada std::function
// alocira.
static int allocations = 0;
void* operator new(std::size_t n) {
    ++allocations;
    if (void* p = std::malloc(n ? n : 1)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }

struct Times {
    int k;
    int operator()(int a, int b) const { return (a + b) * k; }
};

struct Sensor {
    int id_;
    int id() const { return id_; }
    void set(int v) { id_ = v; }
    int withOffset(int offset) const { return id_ + offset; }
};

// ---------------------------------------------------------------- 1
void section1() {
    std::cout << "\n== 1. std::function: one type for everything callable\n";
    std::function<int(int, int)> op;               // prazan
    std::cout << "empty: " << static_cast<bool>(op) << '\n';
    op = add;                                   // funkcija
    std::cout << "function: " << op(2, 3);
    op = Times{10};                                 // funkcijski objekat
    std::cout << ", object: " << op(2, 3);
    op = [](int a, int b) { return a * b; };       // lambda
    std::cout << ", lambda: " << op(2, 3) << '\n';

    // Zato može da bude vrednost u kontejneru: tabela operacija.
    std::map<std::string, std::function<int(int, int)>> operations{
        {"+", add}, {"-", subtract}, {"*", [](int a, int b) { return a * b; }}};
    for (const auto& [name, f] : operations) std::cout << "7 " << name << " 3 = " << f(7, 3) << '\n';

    // Poziv praznog baca std::bad_function_call (runtime/r01 kad ga niko ne uhvati).
    std::function<void()> nothing;
    try {
        nothing();
    } catch (const std::bad_function_call&) {
        std::cout << "empty call: std::bad_function_call\n";
    }
}

// ---------------------------------------------------------------- 2
void section2() {
    std::cout << "\n== 2. std::function: conversions, methods, cost\n";
    // Argumenti i povratna vrednost se konvertuju kao pri običnom pozivu.
    std::function<double(int)> half = [](int x) { return x / 2; };   // int / int!
    std::cout << "half(7) = " << half(7) << " (integer division in the lambda)\n";

    // Metoda: objekat postaje PRVI argument.
    Sensor s{7};
    std::function<int(const Sensor&)> getId = &Sensor::id;
    std::function<void(Sensor&, int)> set = &Sensor::set;
    set(s, 9);
    std::cout << "getId(s) = " << getId(s) << ", std::invoke = " << std::invoke(&Sensor::id, s)
              << '\n';

    // Rekurzivna lambda preko std::function (lambda ne može da imenuje sebe).
    std::function<int(int)> fact = [&fact](int n) { return n <= 1 ? 1 : n * fact(n - 1); };
    std::cout << "fact(5) = " << fact(5) << '\n';

    // Cena: veći objekat, indirektan poziv, a za veliko stanje -- heap.
    std::array<int, 2> small{1, 2};
    std::array<int, 64> large{};
    int before = allocations;
    std::function<int()> f1 = [small] { return small[0]; };
    int afterSmall = allocations;
    std::function<int()> f2 = [large] { return large[0]; };
    int afterLarge = allocations;
    std::cout << "allocations: lambda with 8 B of state " << afterSmall - before << ", with 256 B "
              << afterLarge - afterSmall << "; f1() + f2() = " << f1() + f2() << '\n';
}

// ---------------------------------------------------------------- 3
void section3() {
    std::cout << "\n== 3. std::bind: fixing and reordering arguments\n";
    using namespace std::placeholders;   // _1, _2, ...
    auto add10 = std::bind(add, _1, 10);      // drugi argument fiksiran
    auto reversed = std::bind(subtract, _2, _1);      // zamenjen redosled
    auto always = std::bind(add, 2, 3);           // svi fiksirani: poziva se bez argumenata
    std::cout << "add10(5) = " << add10(5) << ", reversed(10, 3) = " << reversed(10, 3)
              << ", always() = " << always() << '\n';
    // Isto lambdom (EMC Item 34 -- čitljivije, sekcija 5):
    auto add10L = [](int x) { return add(x, 10); };
    std::cout << "lambda: add10L(5) = " << add10L(5) << '\n';
}

// ---------------------------------------------------------------- 4
void addTo(int& counter, int amount) { counter += amount; }

void section4() {
    std::cout << "\n== 4. std::bind: methods, references, mem_fn\n";
    using namespace std::placeholders;
    Sensor s{1};
    // Metoda: drugi argument bind-a je objekat (pokazivač, referenca ili kopija).
    auto setS = std::bind(&Sensor::set, &s, _1);   // pokazivač: menja s
    setS(5);
    auto withOffset = std::bind(&Sensor::withOffset, s, _1); // KOPIJA s-a u trenutku bind-a
    s.set(100);
    std::cout << "s.id() = " << s.id() << ", withOffset(1) on the copy = " << withOffset(1) << '\n';

    // bind KOPIRA argumente -- i one koji idu u referencu (zadatak ex2).
    int counter = 0;
    auto intoCopy = std::bind(addTo, counter, 1);
    auto intoOriginal = std::bind(addTo, std::ref(counter), 1);
    intoCopy();
    intoOriginal();
    intoOriginal();
    std::cout << "counter after 1x copy, 2x std::ref: " << counter << '\n';

    // std::mem_fn: metoda kao funkcijski objekat, objekat je argument.
    std::vector<Sensor> sensors{{3}, {1}, {2}};
    auto id = std::mem_fn(&Sensor::id);
    int total = 0;
    for (const Sensor& x : sensors) total += id(x);
    std::cout << "sum of ids via mem_fn: " << total << '\n';
}

// ---------------------------------------------------------------- 5
int hour = 8;
int currentTime() { return hour; }
int setAlarm(int when) { return when; }

void section5() {
    std::cout << "\n== 5. std::bind: pitfalls, and why lambda\n";
    using namespace std::placeholders;
    auto add10 = std::bind(add, _1, 10);
    // Višak argumenata se tiho IGNORIŠE (kod lambde bi bio greška).
    std::cout << "add10(5, 99, 100) = " << add10(5, 99, 100) << '\n';

    // Argumenti bind-a se računaju ODMAH, a lambda ih računa pri pozivu
    // (zadatak ex3).
    auto alarmBind = std::bind(setAlarm, currentTime() + 1);
    auto alarmLambda = [] { return setAlarm(currentTime() + 1); };
    hour = 12;
    std::cout << "now " << hour << "h -- bind: " << alarmBind() << "h, lambda: " << alarmLambda()
              << "h\n";
    // Preopterećena funkcija se ne može direktno dati bind-u (errors/e01);
    // lambda nema taj problem.
}

int main() {
    std::cout << std::boolalpha;
    section1();
    section2();
    section3();
    section4();
    section5();
}
