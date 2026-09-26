#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <new>
#include <string>
#include <utility>
#include <vector>

// Lekcija 32 -- pametni pokazivači: ISPRAVNI slučajevi. Sve se kompajlira i
// radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior (ili curi)
// ./check_cases.sh 6-smart-pointers/32-smart-pointers  proverava oba.

// Brojač alokacija: zamena globalnog operator new (dozvoljena standardom)
// samo da bi se izmerilo koliko puta make_shared i shared_ptr(new T) idu na
// heap. U običnom kodu se ovo ne radi.
static int allocations = 0;
void* operator new(std::size_t n) {
    ++allocations;
    if (void* p = std::malloc(n)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

struct Widget {
    explicit Widget(int id) : id_(id) { std::cout << "Widget(" << id_ << ") "; }
    ~Widget() { std::cout << "~Widget(" << id_ << ") "; }
    int id() const { return id_; }

private:
    int id_;
};

// ---------------------------------------------------------------- 1
std::unique_ptr<Widget> createWidget(int id) { return std::make_unique<Widget>(id); } // fabrika: vlasništvo ide pozivaocu

void consume(std::unique_ptr<Widget> w) { std::cout << "consume(" << w->id() << ") "; } // preuzima: briše na kraju
void inspect(const Widget& w) { std::cout << "inspect(" << w.id() << ") "; }          // samo koristi
void inspectMaybe(const Widget* w) { std::cout << (w ? "inspectMaybe(ima) " : "inspectMaybe(nullptr) "); }

void s01_uniquePtr() {
    std::cout << "-- 1. unique_ptr: jedan vlasnik (kurs 73, 75) --\n  ";
    auto a = createWidget(1);
    inspect(*a);              // ne-vlasnik: referenca
    inspectMaybe(a.get());    // ne-vlasnik koji može biti prazan: sirov pokazivač
    auto b = std::move(a);    // prenos vlasništva; a je sada nullptr
    std::cout << "| a=" << (a ? "pun" : "prazan") << " b=" << b->id() << "\n  ";
    consume(std::move(b));    // vlasništvo ode u funkciju, Widget umire na njenom kraju
    std::cout << "| posle consume\n  ";
    auto c = createWidget(2);
    Widget* released = c.release(); // c više ne poseduje; ko sada briše? (retko potrebno)
    delete released;
    auto d = createWidget(3);
    d.reset(); // briše sada
    std::cout << "| sizeof(unique_ptr<Widget>)=" << sizeof(std::unique_ptr<Widget>) << " = sizeof(Widget*)\n";
}

// ---------------------------------------------------------------- 2
void s02_sharedPtr() {
    std::cout << "-- 2. shared_ptr: više vlasnika (kurs 74, 76) --\n  ";
    std::shared_ptr<Widget> first = std::make_shared<Widget>(10);
    std::cout << "use_count=" << first.use_count() << " ";
    {
        std::shared_ptr<Widget> second = first; // kopija: +1 (atomski)
        std::vector<std::shared_ptr<Widget>> many(3, first);
        std::cout << "| unutra use_count=" << first.use_count() << " ";
    }
    std::cout << "| posle bloka use_count=" << first.use_count() << " ";
    first.reset(); // poslednji vlasnik -> objekat se briše
    std::cout << "| sizeof(shared_ptr)=" << sizeof(std::shared_ptr<Widget>) << " (pokazivač na objekat + na kontrolni blok)\n";
}

// ---------------------------------------------------------------- 3
struct Plain {
    int value = 0;
};

void s03_makeFunctions() {
    std::cout << "-- 3. make funkcije (kurs 82, EMC Item 21) --\n";
    allocations = 0;
    { auto p = std::make_shared<Plain>(); }
    int viaMake = allocations;
    allocations = 0;
    { std::shared_ptr<Plain> p(new Plain); }
    int viaNew = allocations;
    allocations = 0;
    { auto p = std::make_unique<Plain>(); }
    int viaMakeUnique = allocations;
    std::cout << "  alokacija: make_shared=" << viaMake << " shared_ptr(new T)=" << viaNew
              << " make_unique=" << viaMakeUnique << "  <- make_shared: objekat i kontrolni blok zajedno\n";
}

// ---------------------------------------------------------------- 4
void s04_weakPtr() {
    std::cout << "-- 4. weak_ptr: posmatrač bez vlasništva (kurs 77, 78) --\n  ";
    std::weak_ptr<Widget> observer;
    {
        auto owner = std::make_shared<Widget>(20);
        observer = owner; // ne menja use_count
        std::cout << "use_count=" << owner.use_count() << " expired=" << observer.expired() << " ";
        if (std::shared_ptr<Widget> locked = observer.lock()) { // lock: privremeni vlasnik ili prazno
            std::cout << "lock()->id=" << locked->id() << " ";
        }
    }
    std::cout << "| posle bloka expired=" << observer.expired() << " lock()="
              << (observer.lock() ? "pun" : "prazan") << "\n";
}

// ---------------------------------------------------------------- 5
struct Person {
    explicit Person(std::string n) : name(std::move(n)) {}
    ~Person() { std::cout << "~Person(" << name << ") "; }
    std::string name;
    std::shared_ptr<Person> friendStrong; // NE RADI OVAKO u oba smera: ciklus (ub/u01)
    std::weak_ptr<Person> friendWeak;     // ✅ bar jedan smer weak
};

void s05_cycles() {
    std::cout << "-- 5. kružne reference (kurs 79) --\n  ";
    {
        auto ana = std::make_shared<Person>("Ana");
        auto bojan = std::make_shared<Person>("Bojan");
        ana->friendWeak = bojan;
        bojan->friendWeak = ana;
        std::cout << "weak u oba smera, use_count(ana)=" << ana.use_count() << " | ";
    } // oba se unište
    std::cout << "\n  <- sa shared_ptr u oba smera use_count bi bio 2 i destruktori se ne bi pozvali\n";
}

// ---------------------------------------------------------------- 6
struct FileCloser {
    void operator()(std::FILE* f) const noexcept {
        std::fclose(f);
        std::cout << "[fclose] ";
    }
};

void s06_deleters() {
    std::cout << "-- 6. deleter (kurs 80) --\n  ";
    {
        std::unique_ptr<std::FILE, FileCloser> file(std::tmpfile()); // deleter je deo TIPA
        auto lambdaDeleter = [](Widget* w) {
            std::cout << "[lambda deleter] ";
            delete w;
        };
        std::unique_ptr<Widget, decltype(lambdaDeleter)> w(new Widget(30), lambdaDeleter);
        std::cout << "| ";
    }
    std::cout << "\n  sizeof: unique_ptr<FILE, FileCloser>=" << sizeof(std::unique_ptr<std::FILE, FileCloser>)
              << " unique_ptr<FILE, void(*)(FILE*)>=" << sizeof(std::unique_ptr<std::FILE, void (*)(std::FILE*)>) << "\n  ";
    {
        // shared_ptr: deleter NIJE deo tipa -- isti tip, različiti deleteri.
        std::shared_ptr<Widget> normal = std::make_shared<Widget>(31);
        std::shared_ptr<Widget> custom(new Widget(32), [](Widget* w) {
            std::cout << "[shared deleter] ";
            delete w;
        });
        std::vector<std::shared_ptr<Widget>> both{normal, custom};
        std::cout << "| ";
    }
    std::cout << "\n";
}

// ---------------------------------------------------------------- 7
void s07_arrays() {
    std::cout << "-- 7. dinamički nizovi (kurs 81) --\n";
    auto numbers = std::make_unique<int[]>(4); // unique_ptr<int[]>: delete[] i operator[]
    numbers[2] = 42;
    std::shared_ptr<int[]> shared(new int[3]{1, 2, 3}); // C++17: shared_ptr<T[]> zove delete[]
    std::cout << "  unique_ptr<int[]>: numbers[2]=" << numbers[2] << "; shared_ptr<int[]>: shared[1]=" << shared[1]
              << "  (najčešće je std::vector bolji izbor)\n";
}

// ---------------------------------------------------------------- 8
class Session : public std::enable_shared_from_this<Session> {
public:
    std::shared_ptr<Session> self() { return shared_from_this(); } // NE shared_ptr<Session>(this) (ub/u02)
};

void s08_sharedFromThis() {
    std::cout << "-- 8. enable_shared_from_this --\n";
    auto s = std::make_shared<Session>();
    auto again = s->self();
    std::cout << "  shared_from_this: use_count=" << s.use_count();
    Session onStack;
    try {
        onStack.self(); // objekat nije u shared_ptr-u
    } catch (const std::bad_weak_ptr&) {
        std::cout << "; na objektu van shared_ptr-a -> std::bad_weak_ptr (C++17)";
    }
    std::cout << "\n";
}

int main() {
    std::cout << std::boolalpha;
    s01_uniquePtr();
    s02_sharedPtr();
    s03_makeFunctions();
    s04_weakPtr();
    s05_cycles();
    s06_deleters();
    s07_arrays();
    s08_sharedFromThis();
}
