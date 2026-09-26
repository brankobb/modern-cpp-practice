// Završna vežba dela 4 -- generički kružni bafer
//   ./build.sh 4-templates/capstone/task.cpp
// Uputstvo i spisak lekcija: 4-templates/capstone/notes.md
// Rešenje: solution.cpp (otvori tek kad tvoj izlaz bude isti kao blok
// EXPECTED OUTPUT na dnu fajla).
//
// Fajl se kompajlira i ovakav. Piši redom; posle svakog koraka
// otkomentariši njegov deo main()-a.
//
// Korak 1: template <typename T, std::size_t N> class RingBuffer
//   -- N poslednjih vrednosti u std::array<T, N>; kad je pun, nova
//   prepisuje najstariju. static_assert(N > 0) (lekcija 28, sekcija 8);
//   N je ne-tipski parametar (lekcija 26, sekcija 6).
//   -- capacity() (static constexpr), size(), empty(), full();
//   operator[] (0 = najstariji), at() koji baca std::out_of_range
//   (lekcija 18); forEach(f) od najstarijeg ka najnovijem.
//   -- push(const T&) kopira, push(T&&) premešta (lekcija 22), a
//   template emplace(Args&&...) prosleđuje argumente konstruktoru T preko
//   std::forward (lekcija 27, sekcija 3; lekcija 28, sekcija 1).
//   Tracked broji kopije i premeštanja: proveri da ih je tačno onoliko
//   koliko očekuješ.
// Korak 2: trait IsMeasurement<T> (+ isMeasurement_v) i double valueOf(const T&)
//   -- broj: vrati ga kao double; Measurement: x.value(); sve ostalo:
//   static_assert sa jasnom porukom. Jedna funkcija, if constexpr
//   (lekcija 29, sekcije 4 i 5; lekcija 28, sekcija 7).
//   -- average(const RingBuffer<T, N>&) za bilo koji T za koji radi
//   valueOf.
// Korak 3: variadic i fold (lekcija 29, sekcije 2 i 3)
//   -- pushAll(bafer, xs...): svaki argument prosledi u push, redom.
//   -- bool allInRange(min, max, xs...): da li je svaki u [min, max];
//   bez argumenata je true.
// Korak 4: CTAD, specijalizacije, alias
//   -- template <typename T> struct Range { T min, max; bool contains(const T&) const; }
//   i deduction guide, da bi Range o{0.0, 50.0} radio u C++17 (lekcija 29, sekcija 1).
//   -- Formatter<T>::print(out, x): opšti slučaj x, delimična
//   specijalizacija za T* ("*vrednost" ili "null"), potpuna za
//   std::string (pod navodnicima) (lekcija 28, sekcije 4 i 5).
//   -- print(title, buffer) preko Formatter<T>; alias Buffer4<T> za
//   RingBuffer<T, 4> (lekcija 28, sekcija 6).

#include <array>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

// TODO korak 1: RingBuffer

// Dato: tip koji broji kopije i premeštanja (deo 3).
struct Tracked {
    static inline int copies = 0, moves = 0;
    std::string text;
    Tracked() = default;
    explicit Tracked(std::string t) : text(std::move(t)) {}
    Tracked(const Tracked& o) : text(o.text) { ++copies; }
    Tracked(Tracked&& o) noexcept : text(std::move(o.text)) { ++moves; }
    Tracked& operator=(const Tracked& o) {
        text = o.text;
        ++copies;
        return *this;
    }
    Tracked& operator=(Tracked&& o) noexcept {
        text = std::move(o.text);
        ++moves;
        return *this;
    }
};

// Dato: merenje za korak 2.
struct Measurement {
    std::string channel;
    double v = 0;
    double value() const { return v; }
};

// TODO korak 2, 3, 4

int main() {
    // Korak 1 -- otkomentariši:
    // std::cout << std::boolalpha << "== step 1: buffer, overwriting, copies and moves\n";
    // RingBuffer<int, 3> b;
    // for (int i = 1; i <= 5; ++i) b.push(i);
    // std::cout << "after 1..5:";
    // b.forEach([](int v) { std::cout << ' ' << v; });
    // std::cout << " (size " << b.size() << '/' << b.capacity() << ")\n";
    // std::cout << "full " << b.full() << ", oldest " << b[0] << '\n';
    // try {
    //     b.at(3);
    // } catch (const std::out_of_range& e) {
    //     std::cout << "at(3): " << e.what() << '\n';
    // }
    // RingBuffer<Tracked, 2> p;
    // Tracked x("a");
    // p.push(x);                                           // kopija
    // p.push(Tracked("b"));                                 // premeštanje privremenog
    // p.emplace("c");                                       // prepisuje "a": pravi Tracked, pa ga premesti
    // std::cout << "Tracked: copies " << Tracked::copies << ", moves " << Tracked::moves << ", contents "
    //           << p[0].text << ' ' << p[1].text << '\n';

    // Korak 2 -- otkomentariši:
    // std::cout << "== step 2: average via a trait and if constexpr\n";
    // RingBuffer<Measurement, 3> m;
    // m.emplace(Measurement{"temp", 21.5});
    // m.emplace(Measurement{"temp", 22.5});
    // std::cout << "average int: " << average(b) << ", average of measurements: " << average(m) << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << "== step 3: variadic and fold\n";
    // pushAll(b, 10, 20);
    // std::cout << "after pushAll(10, 20):";
    // b.forEach([](int v) { std::cout << ' ' << v; });
    // std::cout << '\n';
    // std::cout << "allInRange(0, 50, 10, 20.5, measurement 21.5): " << allInRange(0, 50, 10, 20.5, m[0])
    //           << ", with 60: " << allInRange(0, 50, 10, 60) << ", no arguments: " << allInRange(0, 50) << '\n';

    // Korak 4 -- otkomentariši:
    // std::cout << "== step 4: CTAD, specializations, alias\n";
    // Range o{0.0, 50.0};
    // static_assert(std::is_same_v<decltype(o), Range<double>>);
    // std::cout << "Range{0.0, 50.0} contains 21.5: " << o.contains(21.5) << ", 60: " << o.contains(60.0) << '\n';
    // Buffer4<std::string> names;
    // pushAll(names, std::string("temp"), std::string("humidity"));
    // names.emplace(3, 'x');
    // print("string", names);
    // int a = 7, c = 9;
    // RingBuffer<int*, 3> ptrs;
    // pushAll(ptrs, &a, nullptr, &c);
    // print("int*", ptrs);
}

/* EXPECTED OUTPUT
== step 1: buffer, overwriting, copies and moves
after 1..5: 3 4 5 (size 3/3)
full true, oldest 3
at(3): RingBuffer::at: index 3
Tracked: copies 1, moves 2, contents b c
== step 2: average via a trait and if constexpr
average int: 4, average of measurements: 22
== step 3: variadic and fold
after pushAll(10, 20): 5 10 20
allInRange(0, 50, 10, 20.5, measurement 21.5): true, with 60: false, no arguments: true
== step 4: CTAD, specializations, alias
Range{0.0, 50.0} contains 21.5: true, 60: false
string [3/4]: "temp" "humidity" "xxx"
int* [3/3]: *7 null *9
*/
