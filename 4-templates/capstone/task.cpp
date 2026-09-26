// Završna vežba dela 4 -- generički kružni bafer
//   ./build.sh 4-templates/capstone/task.cpp
// Uputstvo i spisak lekcija: 4-templates/capstone/notes.md
// Rešenje: solution.cpp (otvori tek kad tvoj izlaz bude isti kao blok
// EXPECTED OUTPUT na dnu fajla).
//
// Fajl se kompajlira i ovakav. Piši redom; posle svakog koraka
// otkomentariši njegov deo main()-a.
//
// Korak 1: template <typename T, std::size_t N> class KruzniBafer
//   -- N poslednjih vrednosti u std::array<T, N>; kad je pun, nova
//   prepisuje najstariju. static_assert(N > 0) (lekcija 28, sekcija 8);
//   N je ne-tipski parametar (lekcija 26, sekcija 6).
//   -- kapacitet() (static constexpr), size(), prazan(), pun();
//   operator[] (0 = najstariji), at() koji baca std::out_of_range
//   (lekcija 18); zaSvaki(f) od najstarijeg ka najnovijem.
//   -- dodaj(const T&) kopira, dodaj(T&&) premešta (lekcija 22), a
//   template emplace(Args&&...) prosleđuje argumente konstruktoru T preko
//   std::forward (lekcija 27, sekcija 3; lekcija 28, sekcija 1).
//   Pracen broji kopije i premeštanja: proveri da ih je tačno onoliko
//   koliko očekuješ.
// Korak 2: trait JeMerenje<T> (+ jeMerenje_v) i double vrednostOd(const T&)
//   -- broj: vrati ga kao double; Merenje: x.vrednost(); sve ostalo:
//   static_assert sa jasnom porukom. Jedna funkcija, if constexpr
//   (lekcija 29, sekcije 4 i 5; lekcija 28, sekcija 7).
//   -- prosek(const KruzniBafer<T, N>&) za bilo koji T za koji radi
//   vrednostOd.
// Korak 3: variadic i fold (lekcija 29, sekcije 2 i 3)
//   -- dodajSve(bafer, xs...): svaki argument prosledi u dodaj, redom.
//   -- bool sviUOpsegu(min, max, xs...): da li je svaki u [min, max];
//   bez argumenata je true.
// Korak 4: CTAD, specijalizacije, alias
//   -- template <typename T> struct Opseg { T min, max; bool sadrzi(const T&) const; }
//   i deduction guide, da bi Opseg o{0.0, 50.0} radio u C++17 (lekcija 29, sekcija 1).
//   -- Formater<T>::ispisi(out, x): opšti slučaj x, delimična
//   specijalizacija za T* ("*vrednost" ili "null"), potpuna za
//   std::string (pod navodnicima) (lekcija 28, sekcije 4 i 5).
//   -- ispisi(naslov, bafer) preko Formater<T>; alias Bafer4<T> za
//   KruzniBafer<T, 4> (lekcija 28, sekcija 6).

#include <array>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

// TODO korak 1: KruzniBafer

// Dato: tip koji broji kopije i premeštanja (deo 3).
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

// Dato: merenje za korak 2.
struct Merenje {
    std::string kanal;
    double v = 0;
    double vrednost() const { return v; }
};

// TODO korak 2, 3, 4

int main() {
    // Korak 1 -- otkomentariši:
    // std::cout << std::boolalpha << "== korak 1: bafer, prepisivanje, kopije i premeštanja\n";
    // KruzniBafer<int, 3> b;
    // for (int i = 1; i <= 5; ++i) b.dodaj(i);
    // std::cout << "posle 1..5:";
    // b.zaSvaki([](int v) { std::cout << ' ' << v; });
    // std::cout << " (size " << b.size() << '/' << b.kapacitet() << ")\n";
    // std::cout << "pun " << b.pun() << ", najstariji " << b[0] << '\n';
    // try {
    //     b.at(3);
    // } catch (const std::out_of_range& e) {
    //     std::cout << "at(3): " << e.what() << '\n';
    // }
    // KruzniBafer<Pracen, 2> p;
    // Pracen x("a");
    // p.dodaj(x);                                           // kopija
    // p.dodaj(Pracen("b"));                                 // premeštanje privremenog
    // p.emplace("c");                                       // prepisuje "a": pravi Pracen, pa ga premesti
    // std::cout << "Pracen: kopija " << Pracen::kopija << ", premeštanja " << Pracen::premestanja << ", sadržaj "
    //           << p[0].tekst << ' ' << p[1].tekst << '\n';

    // Korak 2 -- otkomentariši:
    // std::cout << "== korak 2: prosek preko trait-a i if constexpr\n";
    // KruzniBafer<Merenje, 3> m;
    // m.emplace(Merenje{"temp", 21.5});
    // m.emplace(Merenje{"temp", 22.5});
    // std::cout << "prosek int: " << prosek(b) << ", prosek merenja: " << prosek(m) << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << "== korak 3: variadic i fold\n";
    // dodajSve(b, 10, 20);
    // std::cout << "posle dodajSve(10, 20):";
    // b.zaSvaki([](int v) { std::cout << ' ' << v; });
    // std::cout << '\n';
    // std::cout << "sviUOpsegu(0, 50, 10, 20.5, merenje 21.5): " << sviUOpsegu(0, 50, 10, 20.5, m[0])
    //           << ", sa 60: " << sviUOpsegu(0, 50, 10, 60) << ", bez argumenata: " << sviUOpsegu(0, 50) << '\n';

    // Korak 4 -- otkomentariši:
    // std::cout << "== korak 4: CTAD, specijalizacije, alias\n";
    // Opseg o{0.0, 50.0};
    // static_assert(std::is_same_v<decltype(o), Opseg<double>>);
    // std::cout << "Opseg{0.0, 50.0} sadrži 21.5: " << o.sadrzi(21.5) << ", 60: " << o.sadrzi(60.0) << '\n';
    // Bafer4<std::string> imena;
    // dodajSve(imena, std::string("temp"), std::string("vlaga"));
    // imena.emplace(3, 'x');
    // ispisi("string", imena);
    // int a = 7, c = 9;
    // KruzniBafer<int*, 3> pok;
    // dodajSve(pok, &a, nullptr, &c);
    // ispisi("int*", pok);
}

/* EXPECTED OUTPUT
== korak 1: bafer, prepisivanje, kopije i premeštanja
posle 1..5: 3 4 5 (size 3/3)
pun true, najstariji 3
at(3): KruzniBafer::at: indeks 3
Pracen: kopija 1, premeštanja 2, sadržaj b c
== korak 2: prosek preko trait-a i if constexpr
prosek int: 4, prosek merenja: 22
== korak 3: variadic i fold
posle dodajSve(10, 20): 5 10 20
sviUOpsegu(0, 50, 10, 20.5, merenje 21.5): true, sa 60: false, bez argumenata: true
== korak 4: CTAD, specijalizacije, alias
Opseg{0.0, 50.0} sadrži 21.5: true, 60: false
string [3/4]: "temp" "vlaga" "xxx"
int* [3/3]: *7 null *9
*/
