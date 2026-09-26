#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

// Lekcija 24 -- copy elision, vraćanje i prosleđivanje po vrednosti:
// ISPRAVNI slučajevi. Sve se kompajlira i radi bez ASan/UBSan prijava
// (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
// ./check_cases.sh 3-zivotni-vek-i-resursi/24-copy-elision  proverava ih.

// Tip koji javlja svaki konstruktor, pa se vidi koliko objekata je napravljeno.
struct Logged {
    Logged() { std::cout << "ctor "; }
    explicit Logged(const char*) { std::cout << "ctor(str) "; }
    Logged(const Logged&) { std::cout << "copy "; }
    Logged(Logged&&) noexcept { std::cout << "move "; }
    Logged& operator=(const Logged&) {
        std::cout << "copy= ";
        return *this;
    }
    Logged& operator=(Logged&&) noexcept {
        std::cout << "move= ";
        return *this;
    }
};

// ---------------------------------------------------------------- 1
Logged makePrvalue() { return Logged(); } // RVO: C++17 GARANTUJE -- nema ni kopije ni move-a

Logged makeNamed() {
    Logged x;
    return x; // NRVO: nije garantovano, ali ga g++ i clang rade i na -O0
}

// NE RADI OVAKO: std::move u return-u sprečava NRVO (EMC Item 25).
// Upozorenje je ovde namerno isključeno da bi se video efekat.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpessimizing-move"
Logged makePessimized() {
    Logged x;
    return std::move(x); // mora move konstruktor
}
#pragma GCC diagnostic pop

void s01_returnValue() {
    std::cout << "-- 1. vraćanje po vrednosti --\n";
    std::cout << "  return Logged();            -> ";
    { Logged a = makePrvalue(); (void)a; }
    std::cout << "\n  Logged x; return x;         -> ";
    { Logged a = makeNamed(); (void)a; }
    std::cout << "\n  Logged x; return move(x);   -> ";
    { Logged a = makePessimized(); (void)a; }
    std::cout << " <- -Wpessimizing-move (g++ i clang, -Wall)\n";
    std::cout << "  Logged a = Logged(Logged(Logged())) -> ";
    { Logged a = Logged(Logged(Logged())); (void)a; } // prvalue se ne materijalizuje dok ne mora: jedan objekat
    std::cout << "\n";
}

// ---------------------------------------------------------------- 2
Logged pickOne(bool first) {
    Logged a;
    Logged b;
    if (first) return a; // dva kandidata -> NRVO nemoguć, ali return lokalne = automatski MOVE
    return b;
}

Logged passThrough(Logged p) { return p; } // parametar: nema elizije, ali automatski move

struct Registry {
    Logged current;
    Logged get() const { return current; } // član: ni lokalna ni parametar -> KOPIJA
};

void s02_whenNoElision() {
    std::cout << "-- 2. kad elizija nije moguća --\n";
    std::cout << "  dve lokalne, vrati jednu    -> ";
    { Logged r = pickOne(true); (void)r; }
    std::cout << "\n  parametar po vrednosti      -> ";
    { Logged r = passThrough(Logged()); (void)r; }
    std::cout << "\n  član objekta (reg.get())    -> ";
    {
        Registry reg; // ctor za reg.current
        Logged r = reg.get();
        (void)r;
    }
    std::cout << "\n  <- lokalna ili parametar: move bez std::move; sve ostalo: kopija\n";
}

// ---------------------------------------------------------------- 3
struct Guarded {
    std::mutex m; // ni kopija ni move
    int value = 7;
};

Guarded makeGuarded() { return Guarded{}; } // C++17: radi, jer nema privremenog objekta (u C++14 greška, errors/e01)

void s03_guaranteedElision() {
    std::cout << "-- 3. garantovana elizija: tip bez kopije i bez move-a --\n";
    Guarded g = makeGuarded();
    std::cout << "  Guarded (std::mutex član) vraćen po vrednosti: value=" << g.value << "\n";
}

// ---------------------------------------------------------------- 4
class Person {
public:
    // Sink parametar po vrednosti + std::move (EMC Item 41): jedna funkcija
    // za lvalue i rvalue.
    void setName(Logged name) { name_ = std::move(name); }

    // Alternativa: dva overload-a, po jedan posao manje za svaki slučaj.
    void setNameRef(const Logged& name) { name_ = name; }
    void setNameRef(Logged&& name) { name_ = std::move(name); }

private:
    Logged name_;
};

void s04_sinkParameters() {
    std::cout << "-- 4. parametar koji se čuva (sink): po vrednosti ili dva overload-a --\n";
    std::cout << "  Person p; Logged lvalue;  -> ";
    Person p;
    Logged lvalue;
    std::cout << "\n  setName(lvalue)          -> ";
    p.setName(lvalue);        // copy u parametar + move= u član
    std::cout << "\n  setName(Logged())        -> ";
    p.setName(Logged());      // ctor + move= (parametar je napravljen direktno, elizija)
    std::cout << "\n  setNameRef(lvalue)       -> ";
    p.setNameRef(lvalue);     // samo copy=
    std::cout << "\n  setNameRef(Logged())     -> ";
    p.setNameRef(Logged());   // ctor + move=
    std::cout << "\n  <- po vrednosti: za lvalue jedan move više, ali jedna funkcija umesto dve\n";
}

// ---------------------------------------------------------------- 5
void s05_emplace() {
    std::cout << "-- 5. push_back vs emplace_back (EMC Item 42) --\n";
    std::vector<Logged> v;
    v.reserve(4);
    std::cout << "  push_back(Logged(\"x\"))  -> ";
    v.push_back(Logged("x")); // privremeni + move u vektor
    std::cout << "\n  emplace_back(\"x\")        -> ";
    v.emplace_back("x");      // konstruiše direktno u vektoru
    std::cout << "\n";
}

int main() {
    s01_returnValue();
    s02_whenNoElision();
    s03_guaranteedElision();
    s04_sinkParameters();
    s05_emplace();
}
