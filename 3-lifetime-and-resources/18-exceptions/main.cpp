#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Izuzeci -- ISPRAVNI slučajevi. Sve se kompajlira bez upozorenja i radi
// bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20). Brojevi
// sekcija prate notes.md.
// POGREŠNI slučajevi:
//   errors/   -- kod koji se NE kompajlira
//   runtime/  -- kod koji se kompajlira, a program se prekine (std::terminate)
// ./check_cases.sh 3-lifetime-and-resources/18-exceptions  proverava oba.

struct Trag {
    explicit Trag(const char* ime) : ime_(ime) { std::cout << ' ' << ime_ << "()"; }
    ~Trag() { std::cout << " ~" << ime_ << "()"; }
    Trag(const Trag&) = delete;
    Trag& operator=(const Trag&) = delete;
    const char* ime_;
};

// ---------------------------------------------------------------- 1
double podeli(double a, double b) {
    if (b == 0.0) throw std::invalid_argument("delilac je 0");
    return a / b;
}

void sekcija1() {
    std::cout << "\n== 1. throw, try, catch\n";
    try {
        std::cout << "10 / 4 = " << podeli(10, 4) << '\n';
        // Od C++17 se << računa sleva nadesno, pa je "10 / 0 = " već ispisano
        // kad podeli() baci. Izuzetak prekida IZRAZ na tom mestu, ne unazad.
        std::cout << "10 / 0 = " << podeli(10, 0) << '\n';
        std::cout << "ovo se ne ispiše\n";
    } catch (const std::invalid_argument& e) {           // hvataj po const&
        std::cout << "uhvaćeno invalid_argument: " << e.what() << '\n';   // ostatak try bloka preskočen
    }
    std::cout << "program ide dalje\n";
}

// ---------------------------------------------------------------- 2
void baci(int koji) {
    switch (koji) {
        case 0: throw std::out_of_range("indeks 7");
        case 1: throw std::runtime_error("senzor ne odgovara");
        case 2: throw std::logic_error("pogrešan redosled poziva");
        default: throw 42;                                 // može i ne-klasa, ali ne treba
    }
}

void sekcija2() {
    std::cout << "\n== 2. više catch blokova: prvi koji odgovara\n";
    for (int i = 0; i < 4; ++i) {
        try {
            baci(i);
        } catch (const std::out_of_range& e) {      // izvedena klasa PRE bazne (logic_error)
            std::cout << "out_of_range: " << e.what() << '\n';
        } catch (const std::logic_error& e) {
            std::cout << "logic_error: " << e.what() << '\n';
        } catch (const std::exception& e) {         // sve standardne
            std::cout << "exception: " << e.what() << '\n';
        } catch (...) {                              // sve ostalo -- uvek poslednji
            std::cout << "nepoznat izuzetak\n";
        }
    }
}

// ---------------------------------------------------------------- 3
// Sopstvena klasa: nasledi standardnu (runtime_error čuva poruku i daje
// what()), dodaj podatke koji pomažu pozivaocu.
class GreskaSenzora : public std::runtime_error {
public:
    GreskaSenzora(int senzorId, const std::string& poruka)
        : std::runtime_error("senzor " + std::to_string(senzorId) + ": " + poruka), id_(senzorId) {}
    int id() const noexcept { return id_; }

private:
    int id_;
};

void sekcija3() {
    std::cout << "\n== 3. sopstvena klasa izuzetka\n";
    try {
        throw GreskaSenzora(7, "van opsega");
    } catch (const GreskaSenzora& e) {
        std::cout << e.what() << " (id " << e.id() << ")\n";
    }
    try {
        throw GreskaSenzora(3, "timeout");
    } catch (const std::exception& e) {              // i kao std::exception -- virtual what()
        std::cout << "kao std::exception: " << e.what() << '\n';
    }
}

// ---------------------------------------------------------------- 4
void unutrasnja() {
    Trag c("c");
    throw std::runtime_error("duboko");
}
void srednja() {
    Trag b("b");
    unutrasnja();
    std::cout << " ovo se ne ispiše";
}

void sekcija4() {
    std::cout << "\n== 4. stack unwinding\n";
    try {
        Trag a("a");
        srednja();
    } catch (const std::exception& e) {
        std::cout << " | uhvaćeno: " << e.what() << '\n';
    }
}

// ---------------------------------------------------------------- 5
void procitajKonfiguraciju() {
    try {
        throw GreskaSenzora(5, "neispravan CRC");
    } catch (const std::exception& e) {
        std::cout << "unutra: zabeleženo (" << e.what() << "), prosleđujem dalje\n";
        throw;   // isti objekat, isti DINAMIČKI tip (throw e; bi napravio kopiju std::exception)
    }
}

void sekcija5() {
    std::cout << "\n== 5. ugnežđeni try i ponovno bacanje\n";
    try {
        procitajKonfiguraciju();
    } catch (const GreskaSenzora& e) {
        std::cout << "spolja: i dalje GreskaSenzora, id " << e.id() << '\n';
    }
}

// ---------------------------------------------------------------- 6
void ucitajBlok(int blok) {
    try {
        throw std::runtime_error("CRC ne odgovara");
    } catch (...) {
        // Dodaj kontekst, a zadrži originalni izuzetak "unutra".
        std::throw_with_nested(std::runtime_error("blok " + std::to_string(blok)));
    }
}

void ucitajFajl() {
    try {
        ucitajBlok(3);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("fajl config.bin"));
    }
}

void ispisiLanac(const std::exception& e, int nivo = 0) {
    std::cout << std::string(static_cast<std::size_t>(nivo) * 2, ' ') << e.what() << '\n';
    try {
        std::rethrow_if_nested(e);           // baci unutrašnji, ako postoji
    } catch (const std::exception& unutra) {
        ispisiLanac(unutra, nivo + 1);
    }
}

void sekcija6() {
    std::cout << "\n== 6. std::nested_exception: lanac uzroka\n";
    try {
        ucitajFajl();
    } catch (const std::exception& e) {
        ispisiLanac(e);
    }
}

// ---------------------------------------------------------------- 7
struct Bafer {
    explicit Bafer(std::size_t n) {
        if (n > 1024) throw std::length_error("bafer prevelik");
        std::cout << " Bafer(" << n << ")";
    }
    ~Bafer() { std::cout << " ~Bafer"; }
};

class Uredjaj {
public:
    // function-try-block: hvata i izuzetke iz init liste. Iz ovog catch-a
    // se NE može "vratiti" (errors/e03): objekat ne postoji, pa izuzetak
    // uvek ide dalje -- ovde preveden u tip koji pozivalac očekuje.
    explicit Uredjaj(std::size_t n) try : trag_("trag"), bafer_(n) {
        std::cout << " telo";
    } catch (const std::length_error& e) {
        std::cout << " | Uredjaj: " << e.what();
        throw GreskaSenzora(0, "Uredjaj nije napravljen");
    }
    ~Uredjaj() { std::cout << " ~Uredjaj"; }

private:
    Trag trag_;
    Bafer bafer_;
};

void sekcija7() {
    std::cout << "\n== 7. konstruktor i destruktor\n";
    try {
        Uredjaj ok(16);
        std::cout << " |";
    } catch (...) {
    }
    std::cout << '\n';
    try {
        Uredjaj los(4096);
    } catch (const GreskaSenzora& e) {
        // trag_ je napravljen, pa uništen PRE nego što handler function-try-
        // block-a počne; ~Uredjaj se NE poziva (objekat nikad nije postojao).
        std::cout << " | spolja: " << e.what() << '\n';
    }
    // Destruktor je implicitno noexcept: izuzetak iz njega = std::terminate
    // (runtime/r02).
    static_assert(std::is_nothrow_destructible_v<Uredjaj>);
}

// ---------------------------------------------------------------- 8
int bezbedna(int x) noexcept { return x * 2; }
int mozeDaBaci(int x) { return x > 0 ? x : throw std::domain_error("x <= 0"); }

template <typename T>
void zameni(T& a, T& b) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                 std::is_nothrow_move_assignable_v<T>) {
    T t = std::move(a);
    a = std::move(b);
    b = std::move(t);
}

struct MoveMozeDaBaci {
    MoveMozeDaBaci() = default;
    MoveMozeDaBaci(MoveMozeDaBaci&&) {}
    MoveMozeDaBaci& operator=(MoveMozeDaBaci&&) { return *this; }
};

void sekcija8() {
    std::cout << "\n== 8. noexcept\n";
    // noexcept(izraz) je OPERATOR: pita pri kompajliranju, ne izvršava izraz.
    std::cout << std::boolalpha;
    std::cout << "noexcept(bezbedna(1)): " << noexcept(bezbedna(1)) << '\n';
    std::cout << "noexcept(mozeDaBaci(1)): " << noexcept(mozeDaBaci(1)) << '\n';
    int a = 1, b = 2;
    MoveMozeDaBaci m1, m2;
    std::cout << "zameni<int> noexcept: " << noexcept(zameni(a, b)) << '\n';
    std::cout << "zameni<MoveMozeDaBaci> noexcept: " << noexcept(zameni(m1, m2)) << '\n';
    std::cout << std::noboolalpha;
    zameni(a, b);
    std::cout << "posle zameni: a=" << a << " b=" << b << '\n';
}

// ---------------------------------------------------------------- 9
std::exception_ptr radi(int x) {
    try {
        mozeDaBaci(x);
        return nullptr;
    } catch (...) {
        return std::current_exception();   // sačuvaj izuzetak kao vrednost
    }
}

void sekcija9() {
    std::cout << "\n== 9. std::exception_ptr\n";
    std::vector<std::exception_ptr> rezultati{radi(5), radi(-1)};
    for (std::size_t i = 0; i < rezultati.size(); ++i) {
        if (!rezultati[i]) {
            std::cout << "posao " << i << ": ok\n";
            continue;
        }
        try {
            std::rethrow_exception(rezultati[i]);   // kasnije, na drugom mestu (npr. druga nit)
        } catch (const std::exception& e) {
            std::cout << "posao " << i << ": " << e.what() << '\n';
        }
    }
}

int main() {
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
