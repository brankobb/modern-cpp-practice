// Rešenje završne vežbe dela 4: generički kružni bafer.

#include <array>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

// ---------------------------------------------------------------- korak 1
// N poslednjih vrednosti; kad je pun, nova prepisuje najstariju.
template <typename T, std::size_t N>
class KruzniBafer {
    static_assert(N > 0, "KruzniBafer: kapacitet mora biti veći od 0");

public:
    static constexpr std::size_t kapacitet() { return N; }
    std::size_t size() const { return broj_; }
    bool prazan() const { return broj_ == 0; }
    bool pun() const { return broj_ == N; }

    void dodaj(const T& x) { mesto() = x; }              // kopija
    void dodaj(T&& x) { mesto() = std::move(x); }        // premeštanje

    // Argumenti idu konstruktoru T, bez usputnih kopija. (Pravi emplace, bez
    // privremenog T, traži sirovu memoriju i placement new: lekcija 33.)
    template <typename... Args>
    T& emplace(Args&&... args) {
        T& m = mesto();
        m = T(std::forward<Args>(args)...);
        return m;
    }

    const T& operator[](std::size_t i) const { return podaci_[(pocetak_ + i) % N]; }   // 0 = najstariji
    const T& at(std::size_t i) const {
        if (i >= broj_) throw std::out_of_range("KruzniBafer::at: indeks " + std::to_string(i));
        return (*this)[i];
    }

    template <typename F>
    void zaSvaki(F&& f) const {
        for (std::size_t i = 0; i < broj_; ++i) f((*this)[i]);
    }

private:
    // Mesto za sledeći element: posle poslednjeg, ili najstariji ako je pun.
    T& mesto() {
        if (broj_ < N) return podaci_[(pocetak_ + broj_++) % N];
        T& m = podaci_[pocetak_];
        pocetak_ = (pocetak_ + 1) % N;
        return m;
    }

    std::array<T, N> podaci_{};
    std::size_t pocetak_ = 0;
    std::size_t broj_ = 0;
};

// Tip koji broji kopije i premeštanja (deo 3).
struct Pracen {
    static inline int kopija = 0, premestanja = 0;
    std::string tekst;
    Pracen() = default;
    explicit Pracen(std::string t) : tekst(std::move(t)) {}
    Pracen(const Pracen& o) : tekst(o.tekst) { ++kopija; }
    Pracen(Pracen&& o) noexcept : tekst(std::move(o.tekst)) { ++premestanja; }
    Pracen& operator=(const Pracen& o) {
        tekst = o.tekst;
        ++kopija;
        return *this;
    }
    Pracen& operator=(Pracen&& o) noexcept {
        tekst = std::move(o.tekst);
        ++premestanja;
        return *this;
    }
};

// ---------------------------------------------------------------- korak 2
struct Merenje {
    std::string kanal;
    double v = 0;
    double vrednost() const { return v; }
};

// Sopstveni trait: koji tipovi su "merenje" (imaju vrednost()).
template <typename T>
struct JeMerenje : std::false_type {};
template <>
struct JeMerenje<Merenje> : std::true_type {};
template <typename T>
inline constexpr bool jeMerenje_v = JeMerenje<T>::value;

template <typename T>
double vrednostOd(const T& x) {
    if constexpr (std::is_arithmetic_v<T>) {
        return static_cast<double>(x);
    } else {
        static_assert(jeMerenje_v<T>, "vrednostOd: tip nije broj ni merenje");
        return x.vrednost();
    }
}

template <typename T, std::size_t N>
double prosek(const KruzniBafer<T, N>& b) {
    if (b.prazan()) return 0;
    double zbir = 0;
    b.zaSvaki([&zbir](const T& x) { zbir += vrednostOd(x); });
    return zbir / static_cast<double>(b.size());
}

// ---------------------------------------------------------------- korak 3
template <typename B, typename... Ts>
void dodajSve(B& b, Ts&&... xs) {
    (b.dodaj(std::forward<Ts>(xs)), ...);                 // fold po zarezu, redom
}

// [[maybe_unused]]: za prazan paket fold ne koristi min i max, i g++ bi
// upozorio "set but not used".
template <typename... Ts>
bool sviUOpsegu([[maybe_unused]] double min, [[maybe_unused]] double max, const Ts&... xs) {
    return ((vrednostOd(xs) >= min && vrednostOd(xs) <= max) && ...);   // prazan paket: true
}

// ---------------------------------------------------------------- korak 4
// Opseg kao agregat: u C++17 CTAD za agregat traži deduction guide.
template <typename T>
struct Opseg {
    T min, max;
    bool sadrzi(const T& x) const { return !(x < min) && !(max < x); }
};
template <typename T>
Opseg(T, T) -> Opseg<T>;

// Formater: opšti slučaj ispiše vrednost, delimična specijalizacija za
// pokazivače ispiše ono na šta pokazuje, a potpuna za std::string stavi navodnike.
template <typename T>
struct Formater {
    static void ispisi(std::ostream& out, const T& x) { out << x; }
};
template <typename T>
struct Formater<T*> {
    static void ispisi(std::ostream& out, T* p) {
        if (p)
            out << '*' << *p;
        else
            out << "null";
    }
};
template <>
struct Formater<std::string> {
    static void ispisi(std::ostream& out, const std::string& s) { out << '"' << s << '"'; }
};

template <typename T, std::size_t N>
void ispisi(const char* naslov, const KruzniBafer<T, N>& b) {
    std::cout << naslov << " [" << b.size() << '/' << b.kapacitet() << "]:";
    b.zaSvaki([](const T& x) {
        std::cout << ' ';
        Formater<T>::ispisi(std::cout, x);
    });
    std::cout << '\n';
}

template <typename T>
using Bafer4 = KruzniBafer<T, 4>;                         // alias šablon

int main() {
    std::cout << std::boolalpha << "== korak 1: bafer, prepisivanje, kopije i premeštanja\n";
    KruzniBafer<int, 3> b;
    for (int i = 1; i <= 5; ++i) b.dodaj(i);
    std::cout << "posle 1..5:";
    b.zaSvaki([](int v) { std::cout << ' ' << v; });
    std::cout << " (size " << b.size() << '/' << b.kapacitet() << ")\n";
    std::cout << "pun " << b.pun() << ", najstariji " << b[0] << '\n';
    try {
        b.at(3);
    } catch (const std::out_of_range& e) {
        std::cout << "at(3): " << e.what() << '\n';
    }
    KruzniBafer<Pracen, 2> p;
    Pracen x("a");
    p.dodaj(x);                                           // kopija
    p.dodaj(Pracen("b"));                                 // premeštanje privremenog
    p.emplace("c");                                       // prepisuje "a": pravi Pracen, pa ga premesti
    std::cout << "Pracen: kopija " << Pracen::kopija << ", premeštanja " << Pracen::premestanja << ", sadržaj "
              << p[0].tekst << ' ' << p[1].tekst << '\n';

    std::cout << "== korak 2: prosek preko trait-a i if constexpr\n";
    KruzniBafer<Merenje, 3> m;
    m.emplace(Merenje{"temp", 21.5});
    m.emplace(Merenje{"temp", 22.5});
    std::cout << "prosek int: " << prosek(b) << ", prosek merenja: " << prosek(m) << '\n';

    std::cout << "== korak 3: variadic i fold\n";
    dodajSve(b, 10, 20);
    std::cout << "posle dodajSve(10, 20):";
    b.zaSvaki([](int v) { std::cout << ' ' << v; });
    std::cout << '\n';
    std::cout << "sviUOpsegu(0, 50, 10, 20.5, merenje 21.5): " << sviUOpsegu(0, 50, 10, 20.5, m[0])
              << ", sa 60: " << sviUOpsegu(0, 50, 10, 60) << ", bez argumenata: " << sviUOpsegu(0, 50) << '\n';

    std::cout << "== korak 4: CTAD, specijalizacije, alias\n";
    Opseg o{0.0, 50.0};
    static_assert(std::is_same_v<decltype(o), Opseg<double>>);
    std::cout << "Opseg{0.0, 50.0} sadrži 21.5: " << o.sadrzi(21.5) << ", 60: " << o.sadrzi(60.0) << '\n';
    Bafer4<std::string> imena;
    dodajSve(imena, std::string("temp"), std::string("vlaga"));
    imena.emplace(3, 'x');
    ispisi("string", imena);
    int a = 7, c = 9;
    KruzniBafer<int*, 3> pok;
    dodajSve(pok, &a, nullptr, &c);
    ispisi("int*", pok);
}
