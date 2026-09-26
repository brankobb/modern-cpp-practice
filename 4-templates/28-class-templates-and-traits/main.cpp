#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Klasni šabloni, variadic šabloni, specijalizacija, alias šabloni, type
// traits, static_assert -- ISPRAVNI slučajevi. Sve se kompajlira bez
// upozorenja i radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i
// C++20). Brojevi sekcija prate notes.md.
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
// ./check_cases.sh 4-templates/28-class-templates-and-traits  proverava ih.

// ---------------------------------------------------------------- 1
// Savršeno prosleđivanje (detaljno: lekcija 27). Ovde u kombinaciji sa
// variadic šablonom: argumenti idu do konstruktora T netaknuti.
struct Senzor {
    Senzor(std::string i, int k) : ime(std::move(i)), kanal(k) {}
    std::string ime;
    int kanal;
};

template <typename T>
class Registar {
public:
    template <typename... Args>
    T& napravi(Args&&... args) {
        elementi_.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return *elementi_.back();
    }
    std::size_t broj() const { return elementi_.size(); }

private:
    std::vector<std::unique_ptr<T>> elementi_;
};

void sekcija1() {
    std::cout << "\n== 1. savršeno prosleđivanje + variadic\n";
    Registar<Senzor> r;
    std::string ime = "temperatura";
    Senzor& a = r.napravi(ime, 1);                  // lvalue: kopira se
    Senzor& b = r.napravi(std::string("pritisak"), 2);   // rvalue: pomera se
    std::cout << a.ime << '/' << a.kanal << ", " << b.ime << '/' << b.kanal
              << ", ime pozivaoca i dalje: " << ime << ", ukupno: " << r.broj() << '\n';
}

// ---------------------------------------------------------------- 2
// Variadic: rekurzija (C++11 stil) -- osnovni slučaj + "prvi i ostali".
void ispisiRek() { std::cout << '\n'; }
template <typename Prvi, typename... Ostali>
void ispisiRek(const Prvi& p, const Ostali&... ostali) {
    std::cout << p << (sizeof...(ostali) ? ", " : "");
    ispisiRek(ostali...);
}

// C++17 fold izrazi -- bez rekurzije.
template <typename... Args>
auto zbir(Args... args) {
    return (0 + ... + args);          // binarni fold sa početnom vrednošću: radi i za 0 argumenata
}
template <typename... Args>
bool sviPozitivni(Args... args) {
    return (... && (args > 0));       // unarni fold; za prazan pack && daje true
}
template <typename... Args>
void ispisiFold(const Args&... args) {
    const char* sep = "";
    ((std::cout << sep << args, sep = ", "), ...);   // fold preko zareza
    std::cout << '\n';
}

int kvadrat(int x) { return x * x; }
template <typename... Args>
int zbirKvadrata(Args... args) {
    return zbir(kvadrat(args)...);    // obrazac se proširi: kvadrat(a1), kvadrat(a2), ...
}

void sekcija2() {
    std::cout << "\n== 2. variadic šabloni\n";
    std::cout << "rekurzija: ";
    ispisiRek(1, 2.5, "tri", 'c');
    std::cout << "fold: ";
    ispisiFold(1, 2.5, "tri", 'c');
    std::cout << "zbir(1, 2, 3, 4) = " << zbir(1, 2, 3, 4) << ", zbir() = " << zbir() << '\n';
    std::cout << "sviPozitivni(3, 5, -1) = " << sviPozitivni(3, 5, -1) << ", sviPozitivni() = "
              << sviPozitivni() << '\n';
    std::cout << "zbirKvadrata(1, 2, 3) = " << zbirKvadrata(1, 2, 3) << '\n';
}

// ---------------------------------------------------------------- 3
// Klasni šablon: stek fiksnog kapaciteta, bez heap-a (embedded).
template <typename T, std::size_t N>
class Stek {
public:
    void push(const T& v) {
        if (vel_ == N) throw std::overflow_error("Stek je pun");
        podaci_[vel_++] = v;
    }
    T pop();                          // definicija van klase, ispod
    std::size_t velicina() const { return vel_; }
    static constexpr std::size_t kapacitet() { return N; }

    // Instancira se tek kad se POZOVE -- Stek<T> za T bez operator< je
    // ispravan dok se najveci() ne koristi (errors/e04).
    T najveci() const {
        T m = podaci_[0];
        for (std::size_t i = 1; i < vel_; ++i)
            if (m < podaci_[i]) m = podaci_[i];
        return m;
    }

private:
    std::array<T, N> podaci_{};
    std::size_t vel_ = 0;
};

template <typename T, std::size_t N>   // van klase: ponovi parametre i Stek<T, N>::
T Stek<T, N>::pop() {
    if (vel_ == 0) throw std::underflow_error("Stek je prazan");
    return podaci_[--vel_];
}

struct Tacka {
    int x = 0, y = 0;
};   // nema operator<

// CTAD (C++17): argumenti klasnog šablona iz konstruktora.
template <typename T>
struct Omotac {
    explicit Omotac(T v) : vrednost(std::move(v)) {}
    T vrednost;
};
Omotac(const char*)->Omotac<std::string>;   // deduction guide: literal -> std::string

void sekcija3() {
    std::cout << "\n== 3. klasni šabloni\n";
    Stek<int, 4> s;
    s.push(3);
    s.push(9);
    s.push(5);
    std::cout << "velicina " << s.velicina() << "/" << Stek<int, 4>::kapacitet() << ", najveci "
              << s.najveci() << ", pop " << s.pop() << '\n';

    Stek<Tacka, 2> t;                 // bez operator< -- u redu, najveci() se ne zove
    t.push({1, 2});
    std::cout << "Stek<Tacka>: pop -> (" << t.pop().x << ", ...)\n";

    std::pair p{1, 2.5};              // std::pair<int, double>
    std::array a{1, 2, 3};            // std::array<int, 3>
    Omotac o{"tekst"};                // Omotac<std::string>, zbog deduction guide-a
    static_assert(std::is_same_v<decltype(p), std::pair<int, double>>);
    static_assert(std::is_same_v<decltype(a), std::array<int, 3>>);
    static_assert(std::is_same_v<decltype(o), Omotac<std::string>>);
    std::cout << "CTAD: pair " << p.first << '/' << p.second << ", array " << a.size()
              << ", Omotac<std::string> dužina " << o.vrednost.size() << '\n';
}

// ---------------------------------------------------------------- 4
// Eksplicitna (potpuna) specijalizacija klasnog šablona.
template <typename T>
struct Opis {
    static std::string ime() { return "nešto"; }
};
template <>
struct Opis<bool> {                   // potpuno drugačija klasa za bool
    static std::string ime() { return "bool"; }
};
template <>
struct Opis<int> {
    static std::string ime() { return "int"; }
};

// Specijalizacija samo jedne metode, ostatak klase ostaje opšti.
template <typename T>
struct Kutija {
    T v;
    std::string prikazi() const { return std::to_string(v); }
    bool prazna() const { return false; }
};
template <>
std::string Kutija<std::string>::prikazi() const {
    return '"' + v + '"';
}

void sekcija4() {
    std::cout << "\n== 4. eksplicitna specijalizacija klase\n";
    std::cout << "Opis<bool>: " << Opis<bool>::ime() << ", Opis<int>: " << Opis<int>::ime()
              << ", Opis<double>: " << Opis<double>::ime() << '\n';
    Kutija<int> k1{42};
    Kutija<std::string> k2{"zdravo"};
    std::cout << "Kutija<int>: " << k1.prikazi() << ", Kutija<std::string>: " << k2.prikazi()
              << ", prazna: " << k2.prazna() << '\n';
}

// ---------------------------------------------------------------- 5
// Delimična specijalizacija: za CELU FAMILIJU tipova.
template <typename T>
struct Opis<T*> {
    static std::string ime() { return "pokazivač na " + Opis<T>::ime(); }
};
template <typename T>
struct Opis<std::vector<T>> {
    static std::string ime() { return "vektor od " + Opis<T>::ime(); }
};
template <typename T, std::size_t N>
struct Opis<T[N]> {
    static std::string ime() { return "niz od " + std::to_string(N) + " x " + Opis<T>::ime(); }
};

void sekcija5() {
    std::cout << "\n== 5. delimična specijalizacija\n";
    std::cout << Opis<int*>::ime() << '\n';
    std::cout << Opis<std::vector<bool*>>::ime() << '\n';
    std::cout << Opis<int[4]>::ime() << '\n';
    std::cout << Opis<double**>::ime() << '\n';
}

// ---------------------------------------------------------------- 6
// Alias šabloni (C++11): typedef ne može da ima parametre, using može.
template <std::size_t N>
using Bajtovi = std::array<std::uint8_t, N>;
template <typename T>
using PoImenu = std::map<std::string, T>;
using Paket = Bajtovi<8>;             // obični alias: isto što i typedef

void sekcija6() {
    std::cout << "\n== 6. alias šabloni\n";
    Paket p{0x01, 0x02};
    PoImenu<int> kanali{{"temp", 1}, {"pritisak", 2}};
    static_assert(std::is_same_v<Paket, std::array<std::uint8_t, 8>>);   // alias NIJE nov tip
    std::cout << "Paket: " << p.size() << " bajtova, prvi " << int(p[0]) << "; kanal pritiska: "
              << kanali["pritisak"] << '\n';
}

// ---------------------------------------------------------------- 7
// Type traits: pitanja o tipu (i transformacije) pri kompajliranju.
// Sopstveni trait: primarni šablon "ne", delimična specijalizacija "da".
template <typename T>
struct jeVektor : std::false_type {};
template <typename T>
struct jeVektor<std::vector<T>> : std::true_type {};
template <typename T>
inline constexpr bool jeVektor_v = jeVektor<T>::value;   // _v pomoćnik (variable template)

template <typename T>
std::string velicinaIliVrednost(const T& x) {
    if constexpr (jeVektor_v<T>)
        return "vektor, " + std::to_string(x.size()) + " elem.";
    else if constexpr (std::is_arithmetic_v<T>)
        return "broj " + std::to_string(x);
    else
        return "nešto drugo";
}

void sekcija7() {
    std::cout << "\n== 7. type traits\n";
    static_assert(std::is_integral_v<int> && !std::is_integral_v<double>);
    static_assert(std::is_same_v<std::remove_reference_t<const int&>, const int>);
    static_assert(std::is_same_v<std::remove_cv_t<std::remove_reference_t<const int&>>, int>);
    static_assert(std::is_same_v<std::decay_t<const int&>, int>);
    static_assert(std::is_same_v<std::decay_t<int[3]>, int*>);
    static_assert(std::is_same_v<std::conditional_t<(sizeof(int) >= 4), int, long>, int>);
    static_assert(std::is_same_v<std::common_type_t<int, double>, double>);
    static_assert(jeVektor_v<std::vector<int>> && !jeVektor_v<int>);
    std::cout << velicinaIliVrednost(std::vector<int>{1, 2, 3}) << "; " << velicinaIliVrednost(7)
              << "; " << velicinaIliVrednost(std::string("x")) << '\n';
}

// ---------------------------------------------------------------- 8
// static_assert u šablonu: jasna poruka na mestu upotrebe, umesto greške
// duboko u telu (errors/e03).
template <typename T>
class Merenje {
    static_assert(std::is_arithmetic_v<T>, "Merenje<T>: T mora biti brojčani tip");

public:
    explicit Merenje(T v) : v_(v) {}
    T vrednost() const { return v_; }

private:
    T v_;
};

void sekcija8() {
    std::cout << "\n== 8. static_assert\n";
    Merenje<double> m(21.5);
    static_assert(sizeof(Merenje<std::uint16_t>) == 2);   // C++17: poruka nije obavezna
    std::cout << "Merenje<double>: " << m.vrednost() << '\n';
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
}
