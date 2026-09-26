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
// ./check_cases.sh 5-functions-as-values/30-lambdas  proverava oba.

// ---------------------------------------------------------------- 1
// Callback kao pokazivač na funkciju (lekcija 11, sekcija 7): bez stanja.
bool isEven(int x) { return x % 2 == 0; }

int countMatching(const std::vector<int>& v, bool (*pred)(int)) {
    int n = 0;
    for (int x : v)
        if (pred(x)) ++n;
    return n;
}

void section1() {
    std::cout << "\n== 1. callback: pointer to function\n";
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    std::cout << "even: " << countMatching(v, isEven) << '\n';
    // Ograničenje: funkcija nema stanje. "Veći od praga" bi tražio globalnu
    // promenljivu za prag.
}

// ---------------------------------------------------------------- 2
// Funkcijski objekat (lekcija 15, sekcija 9): klasa sa operator() -- ima stanje.
class GreaterThan {
public:
    explicit GreaterThan(int threshold) : threshold_(threshold) {}
    bool operator()(int x) const { return x > threshold_; }

private:
    int threshold_;
};

void section2() {
    std::cout << "\n== 2. callback: function object\n";
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    std::cout << "greater than 4: " << std::count_if(v.begin(), v.end(), GreaterThan(4)) << '\n';
}

// ---------------------------------------------------------------- 3
void section3() {
    std::cout << "\n== 3. lambda expression\n";
    std::vector<int> v{5, -3, 8, -1, 2};
    int threshold = 4;
    // Isto što i GreaterThan(threshold), ali na mestu upotrebe, bez posebne klase.
    std::cout << "greater than " << threshold << ": "
              << std::count_if(v.begin(), v.end(), [threshold](int x) { return x > threshold; }) << '\n';
    // Sortiranje po apsolutnoj vrednosti.
    std::sort(v.begin(), v.end(), [](int a, int b) { return std::abs(a) < std::abs(b); });
    std::cout << "by |x|:";
    for (int x : v) std::cout << ' ' << x;
    std::cout << '\n';
    // Povratni tip se izvodi iz return-a; kad ih je više različitih, napiši ga.
    auto divide = [](int a, int b) -> double {
        if (b == 0) return 0;      // int
        return double(a) / b;      // double -- bez "-> double" bila bi greška
    };
    std::cout << "divide(7, 2) = " << divide(7, 2) << '\n';
}

// ---------------------------------------------------------------- 4
// Šta kompajler napravi od [threshold](int x) { return x > threshold; } -- otprilike:
//   class __lambda_1 {
//       int threshold;                                        // zarobljena kopija
//   public:
//       bool operator()(int x) const { return x > threshold; }   // const!
//   };
void section4() {
    std::cout << "\n== 4. how a lambda works inside\n";
    int a = 1;
    double d = 2;
    auto none = [] { return 1; };
    auto one = [a] { return a; };
    auto two = [a, d] { return a + d; };
    auto ref = [&a, &d] { return a + d; };
    // Veličina = veličina zarobljenih vrednosti (i poravnanje).
    std::cout << "sizeof: no capture " << sizeof(none) << ", [a] " << sizeof(one) << ", [a, d] "
              << sizeof(two) << ", [&a, &d] " << sizeof(ref) << '\n';
    // Bez capture-a: konvertuje se u pokazivač na funkciju.
    int (*fp)() = none;
    std::cout << "no capture as a pointer: " << fp() << '\n';
    // constexpr lambda (C++17): radi i pri kompajliranju.
    constexpr auto square = [](int x) { return x * x; };
    static_assert(square(3) == 9);
    std::cout << "one() + two() + ref() = " << one() + two() + ref() << '\n';
}

// ---------------------------------------------------------------- 5
void section5() {
    std::cout << "\n== 5. capture: by value and by reference\n";
    int x = 1;
    auto byValue = [x] { return x; };     // kopija U TRENUTKU pravljenja lambde
    auto byRef = [&x] { return x; };    // referenca: vidi kasnije promene
    x = 2;
    std::cout << "by value: " << byValue() << ", by reference: " << byRef() << '\n';
    // Menjanje kopije traži mutable (operator() je inače const, lekcija 09).
    auto counter = [n = 0]() mutable { return ++n; };
    counter();
    std::cout << "mutable counter: " << counter() << '\n';
}

// ---------------------------------------------------------------- 6
int globalValue = 100;

void section6() {
    std::cout << "\n== 6. default capture: [=] and [&]\n";
    int a = 1, b = 2, sum = 0;
    auto f1 = [=] { return a + b; };                  // sve što koristi, po vrednosti
    auto f2 = [&] { sum = a + b; };                  // sve po referenci
    auto f3 = [=, &sum] { sum = a * 10 + b; };      // sve po vrednosti, sum po referenci
    auto f4 = [&, a] { sum = a + b + 100; };         // sve po referenci, a po vrednosti
    f2();
    std::cout << "f1 " << f1() << ", after f2 sum " << sum;
    f3();
    std::cout << ", after f3 " << sum;
    f4();
    std::cout << ", after f4 " << sum << '\n';
    // Globalne i static promenljive se NE zarobljavaju -- koriste se direktno
    // (errors/e06). Lambda vidi njihovu trenutnu vrednost.
    auto g = [] { return globalValue; };
    globalValue = 200;
    std::cout << "global from the lambda: " << g() << '\n';
}

// ---------------------------------------------------------------- 7
class Thermostat {
public:
    explicit Thermostat(int threshold) : threshold_(threshold) {}
    void setThreshold(int p) { threshold_ = p; }

    // [this]: zarobi POKAZIVAČ -- lambda vidi trenutni threshold_, i sme da
    // se koristi samo dok objekat živi (ub/u01).
    auto liveCheck() const {
        return [this](int t) { return t < threshold_; };
    }
    // [*this] (C++17): zarobi KOPIJU objekta -- nezavisna od originala.
    auto copyCheck() const {
        return [*this](int t) { return t < threshold_; };
    }
    // Samo ono što treba: init capture člana.
    auto thresholdCheck() const {
        return [threshold = threshold_](int t) { return t < threshold; };
    }

private:
    int threshold_;
};

void section7() {
    std::cout << "\n== 7. capture and this\n";
    Thermostat ts(20);
    auto live = ts.liveCheck();
    auto copy = ts.copyCheck();
    auto threshold = ts.thresholdCheck();
    ts.setThreshold(10);
    std::cout << "15 < threshold? [this]: " << live(15) << ", [*this]: " << copy(15)
              << ", [threshold = threshold_]: " << threshold(15) << '\n';
}

// ---------------------------------------------------------------- 8
void section8() {
    std::cout << "\n== 8. generalized capture (C++14)\n";
    // Nova promenljiva u lambdi, inicijalizovana izrazom.
    std::string name = "sensor";
    auto describe = [text = name + "-01", length = name.size()] { return text + "/" + std::to_string(length); };
    std::cout << describe() << '\n';
    // Move-only objekat se PREMESTI u lambdu ([p] bi tražio kopiju, errors/e04).
    auto p = std::make_unique<int>(42);
    auto owner = [q = std::move(p)] { return *q; };
    std::cout << "from unique_ptr: " << owner() << ", p after: " << (p ? "full" : "empty") << '\n';
    // Takva lambda nema kopiju, pa ne može u std::function (errors/e05) --
    // čuva se kao auto, ili prosleđuje šablonu.
    auto callIt = [](auto&& f) { return f(); };
    std::cout << "via a template: " << callIt(owner) << '\n';
}

// ---------------------------------------------------------------- 9
void section9() {
    std::cout << "\n== 9. generic lambdas, std::function, IIFE\n";
    auto add = [](auto a, auto b) { return a + b; };   // operator() je šablon
    std::cout << "add(1, 2) = " << add(1, 2) << ", add(1.5, 2) = " << add(1.5, 2)
              << ", add(string) = " << add(std::string("a"), "b") << '\n';

    // std::function: jedan tip za bilo šta što se poziva -- po cenu veličine
    // i indirektnog poziva (i moguće alokacije za veliko stanje).
    std::vector<std::function<int(int)>> steps;
    steps.push_back([](int x) { return x + 1; });
    steps.push_back(GreaterThan(0));                  // bool -> int
    int factor = 3;
    steps.push_back([factor](int x) { return x * factor; });
    std::cout << "results for 5:";
    for (const auto& k : steps) std::cout << ' ' << k(5);
    std::cout << '\n';

    // IIFE: lambda pozvana odmah -- za const promenljivu kojoj treba više
    // koraka da se izračuna.
    const std::vector<int> squares = [] {
        std::vector<int> v;
        for (int i = 1; i <= 4; ++i) v.push_back(i * i);
        return v;
    }();
    std::cout << "squares.size() = " << squares.size() << ", last " << squares.back() << '\n';
}

int main() {
    std::cout << std::boolalpha;
    section1();
    section2();
    section3();
    section4();
    section5();
    section6();
    section7();
    section8();
    section9();
}
