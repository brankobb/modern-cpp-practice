#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Lambde -- ISPRAVNI slučajevi. Sve se kompajlira bez upozorenja i radi
// bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20). Brojevi
// sekcija prate notes.md.
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh week3-advanced/s14-lambdas  proverava oba.

// ---------------------------------------------------------------- 1
// Callback kao pokazivač na funkciju (lekcija 09, sekcija 7): bez stanja.
bool paran(int x) { return x % 2 == 0; }

int prebroj(const std::vector<int>& v, bool (*uslov)(int)) {
    int n = 0;
    for (int x : v)
        if (uslov(x)) ++n;
    return n;
}

void sekcija1() {
    std::cout << "\n== 1. callback: pokazivač na funkciju\n";
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    std::cout << "parnih: " << prebroj(v, paran) << '\n';
    // Ograničenje: funkcija nema stanje. "Veći od praga" bi tražio globalnu
    // promenljivu za prag.
}

// ---------------------------------------------------------------- 2
// Funkcijski objekat (lekcija 12, sekcija 9): klasa sa operator() -- ima stanje.
class VeciOd {
public:
    explicit VeciOd(int prag) : prag_(prag) {}
    bool operator()(int x) const { return x > prag_; }

private:
    int prag_;
};

void sekcija2() {
    std::cout << "\n== 2. callback: funkcijski objekat\n";
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    std::cout << "veći od 4: " << std::count_if(v.begin(), v.end(), VeciOd(4)) << '\n';
}

// ---------------------------------------------------------------- 3
void sekcija3() {
    std::cout << "\n== 3. lambda izraz\n";
    std::vector<int> v{5, -3, 8, -1, 2};
    int prag = 4;
    // Isto što i VeciOd(prag), ali na mestu upotrebe, bez posebne klase.
    std::cout << "veći od " << prag << ": "
              << std::count_if(v.begin(), v.end(), [prag](int x) { return x > prag; }) << '\n';
    // Sortiranje po apsolutnoj vrednosti.
    std::sort(v.begin(), v.end(), [](int a, int b) { return std::abs(a) < std::abs(b); });
    std::cout << "po |x|:";
    for (int x : v) std::cout << ' ' << x;
    std::cout << '\n';
    // Povratni tip se izvodi iz return-a; kad ih je više različitih, napiši ga.
    auto podeli = [](int a, int b) -> double {
        if (b == 0) return 0;      // int
        return double(a) / b;      // double -- bez "-> double" bila bi greška
    };
    std::cout << "podeli(7, 2) = " << podeli(7, 2) << '\n';
}

// ---------------------------------------------------------------- 4
// Šta kompajler napravi od [prag](int x) { return x > prag; } -- otprilike:
//   class __lambda_1 {
//       int prag;                                        // zarobljena kopija
//   public:
//       bool operator()(int x) const { return x > prag; }   // const!
//   };
void sekcija4() {
    std::cout << "\n== 4. kako lambda radi iznutra\n";
    int a = 1;
    double d = 2;
    auto bez = [] { return 1; };
    auto jedan = [a] { return a; };
    auto dva = [a, d] { return a + d; };
    auto ref = [&a, &d] { return a + d; };
    // Veličina = veličina zarobljenih vrednosti (i poravnanje).
    std::cout << "sizeof: bez capture-a " << sizeof(bez) << ", [a] " << sizeof(jedan) << ", [a, d] "
              << sizeof(dva) << ", [&a, &d] " << sizeof(ref) << '\n';
    // Bez capture-a: konvertuje se u pokazivač na funkciju.
    int (*fp)() = bez;
    std::cout << "bez capture-a kao pokazivač: " << fp() << '\n';
    // constexpr lambda (C++17): radi i pri kompajliranju.
    constexpr auto kvadrat = [](int x) { return x * x; };
    static_assert(kvadrat(3) == 9);
    std::cout << "jedan() + dva() + ref() = " << jedan() + dva() + ref() << '\n';
}

// ---------------------------------------------------------------- 5
void sekcija5() {
    std::cout << "\n== 5. capture: po vrednosti i po referenci\n";
    int x = 1;
    auto poVrednosti = [x] { return x; };     // kopija U TRENUTKU pravljenja lambde
    auto poReferenci = [&x] { return x; };    // referenca: vidi kasnije promene
    x = 2;
    std::cout << "po vrednosti: " << poVrednosti() << ", po referenci: " << poReferenci() << '\n';
    // Menjanje kopije traži mutable (operator() je inače const, lekcija 07).
    auto brojac = [n = 0]() mutable { return ++n; };
    brojac();
    std::cout << "mutable brojač: " << brojac() << '\n';
}

// ---------------------------------------------------------------- 6
int globalni = 100;

void sekcija6() {
    std::cout << "\n== 6. podrazumevani capture: [=] i [&]\n";
    int a = 1, b = 2, zbir = 0;
    auto f1 = [=] { return a + b; };                  // sve što koristi, po vrednosti
    auto f2 = [&] { zbir = a + b; };                  // sve po referenci
    auto f3 = [=, &zbir] { zbir = a * 10 + b; };      // sve po vrednosti, zbir po referenci
    auto f4 = [&, a] { zbir = a + b + 100; };         // sve po referenci, a po vrednosti
    f2();
    std::cout << "f1 " << f1() << ", posle f2 zbir " << zbir;
    f3();
    std::cout << ", posle f3 " << zbir;
    f4();
    std::cout << ", posle f4 " << zbir << '\n';
    // Globalne i static promenljive se NE zarobljavaju -- koriste se direktno
    // (errors/e06). Lambda vidi njihovu trenutnu vrednost.
    auto g = [] { return globalni; };
    globalni = 200;
    std::cout << "globalni iz lambde: " << g() << '\n';
}

// ---------------------------------------------------------------- 7
class Termostat {
public:
    explicit Termostat(int prag) : prag_(prag) {}
    void postaviPrag(int p) { prag_ = p; }

    // [this]: zarobi POKAZIVAČ -- lambda vidi trenutni prag_, i sme da
    // se koristi samo dok objekat živi (ub/u01).
    auto proveraUzivo() const {
        return [this](int t) { return t < prag_; };
    }
    // [*this] (C++17): zarobi KOPIJU objekta -- nezavisna od originala.
    auto proveraKopija() const {
        return [*this](int t) { return t < prag_; };
    }
    // Samo ono što treba: init capture člana.
    auto proveraPrag() const {
        return [prag = prag_](int t) { return t < prag; };
    }

private:
    int prag_;
};

void sekcija7() {
    std::cout << "\n== 7. capture i this\n";
    Termostat ts(20);
    auto uzivo = ts.proveraUzivo();
    auto kopija = ts.proveraKopija();
    auto prag = ts.proveraPrag();
    ts.postaviPrag(10);
    std::cout << "15 < prag? [this]: " << uzivo(15) << ", [*this]: " << kopija(15)
              << ", [prag = prag_]: " << prag(15) << '\n';
}

// ---------------------------------------------------------------- 8
void sekcija8() {
    std::cout << "\n== 8. generalizovani capture (C++14)\n";
    // Nova promenljiva u lambdi, inicijalizovana izrazom.
    std::string ime = "senzor";
    auto opis = [tekst = ime + "-01", duzina = ime.size()] { return tekst + "/" + std::to_string(duzina); };
    std::cout << opis() << '\n';
    // Move-only objekat se PREMESTI u lambdu ([p] bi tražio kopiju, errors/e04).
    auto p = std::make_unique<int>(42);
    auto vlasnik = [q = std::move(p)] { return *q; };
    std::cout << "iz unique_ptr-a: " << vlasnik() << ", p posle: " << (p ? "pun" : "prazan") << '\n';
    // Takva lambda nema kopiju, pa ne može u std::function (errors/e05) --
    // čuva se kao auto, ili prosleđuje šablonu.
    auto pozovi = [](auto&& f) { return f(); };
    std::cout << "preko šablona: " << pozovi(vlasnik) << '\n';
}

// ---------------------------------------------------------------- 9
void sekcija9() {
    std::cout << "\n== 9. generičke lambde, std::function, IIFE\n";
    auto saberi = [](auto a, auto b) { return a + b; };   // operator() je šablon
    std::cout << "saberi(1, 2) = " << saberi(1, 2) << ", saberi(1.5, 2) = " << saberi(1.5, 2)
              << ", saberi(string) = " << saberi(std::string("a"), "b") << '\n';

    // std::function: jedan tip za bilo šta što se poziva -- po cenu veličine
    // i indirektnog poziva (i moguće alokacije za veliko stanje).
    std::vector<std::function<int(int)>> koraci;
    koraci.push_back([](int x) { return x + 1; });
    koraci.push_back(VeciOd(0));                  // bool -> int
    int faktor = 3;
    koraci.push_back([faktor](int x) { return x * faktor; });
    std::cout << "rezultati za 5:";
    for (const auto& k : koraci) std::cout << ' ' << k(5);
    std::cout << '\n';

    // IIFE: lambda pozvana odmah -- za const promenljivu kojoj treba više
    // koraka da se izračuna.
    const std::vector<int> kvadrati = [] {
        std::vector<int> v;
        for (int i = 1; i <= 4; ++i) v.push_back(i * i);
        return v;
    }();
    std::cout << "kvadrati.size() = " << kvadrati.size() << ", poslednji " << kvadrati.back() << '\n';
}

int main() {
    std::cout << std::boolalpha;
    sekcija1();
    sekcija2();
    sekcija3();
    sekcija4();
    sekcija5();
    sekcija6();
    sekcija7();
    sekcija8();
    sekcija9();
}
