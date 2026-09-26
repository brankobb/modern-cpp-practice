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
class RingBuffer {
    static_assert(N > 0, "RingBuffer: capacity must be greater than 0");

public:
    static constexpr std::size_t capacity() { return N; }
    std::size_t size() const { return count_; }
    bool empty() const { return count_ == 0; }
    bool full() const { return count_ == N; }

    void push(const T& x) { slot() = x; }              // kopija
    void push(T&& x) { slot() = std::move(x); }        // premeštanje

    // Argumenti idu konstruktoru T, bez usputnih kopija. (Pravi emplace, bez
    // privremenog T, traži sirovu memoriju i placement new: lekcija 33.)
    template <typename... Args>
    T& emplace(Args&&... args) {
        T& m = slot();
        m = T(std::forward<Args>(args)...);
        return m;
    }

    const T& operator[](std::size_t i) const { return data_[(start_ + i) % N]; }   // 0 = najstariji
    const T& at(std::size_t i) const {
        if (i >= count_) throw std::out_of_range("RingBuffer::at: index " + std::to_string(i));
        return (*this)[i];
    }

    template <typename F>
    void forEach(F&& f) const {
        for (std::size_t i = 0; i < count_; ++i) f((*this)[i]);
    }

private:
    // Mesto za sledeći element: posle poslednjeg, ili najstariji ako je pun.
    T& slot() {
        if (count_ < N) return data_[(start_ + count_++) % N];
        T& m = data_[start_];
        start_ = (start_ + 1) % N;
        return m;
    }

    std::array<T, N> data_{};
    std::size_t start_ = 0;
    std::size_t count_ = 0;
};

// Tip koji broji kopije i premeštanja (deo 3).
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

// ---------------------------------------------------------------- korak 2
struct Measurement {
    std::string channel;
    double v = 0;
    double value() const { return v; }
};

// Sopstveni trait: koji tipovi su "merenje" (imaju vrednost()).
template <typename T>
struct IsMeasurement : std::false_type {};
template <>
struct IsMeasurement<Measurement> : std::true_type {};
template <typename T>
inline constexpr bool isMeasurement_v = IsMeasurement<T>::value;

template <typename T>
double valueOf(const T& x) {
    if constexpr (std::is_arithmetic_v<T>) {
        return static_cast<double>(x);
    } else {
        static_assert(isMeasurement_v<T>, "valueOf: type is neither a number nor a measurement");
        return x.value();
    }
}

template <typename T, std::size_t N>
double average(const RingBuffer<T, N>& b) {
    if (b.empty()) return 0;
    double total = 0;
    b.forEach([&total](const T& x) { total += valueOf(x); });
    return total / static_cast<double>(b.size());
}

// ---------------------------------------------------------------- korak 3
template <typename B, typename... Ts>
void pushAll(B& b, Ts&&... xs) {
    (b.push(std::forward<Ts>(xs)), ...);                 // fold po zarezu, redom
}

// [[maybe_unused]]: za prazan paket fold ne koristi min i max, i g++ bi
// upozorio "set but not used".
template <typename... Ts>
bool allInRange([[maybe_unused]] double min, [[maybe_unused]] double max, const Ts&... xs) {
    return ((valueOf(xs) >= min && valueOf(xs) <= max) && ...);   // prazan paket: true
}

// ---------------------------------------------------------------- korak 4
// Range kao agregat: u C++17 CTAD za agregat traži deduction guide.
template <typename T>
struct Range {
    T min, max;
    bool contains(const T& x) const { return !(x < min) && !(max < x); }
};
template <typename T>
Range(T, T) -> Range<T>;

// Formatter: opšti slučaj ispiše vrednost, delimična specijalizacija za
// pokazivače ispiše ono na šta pokazuje, a potpuna za std::string stavi navodnike.
template <typename T>
struct Formatter {
    static void print(std::ostream& out, const T& x) { out << x; }
};
template <typename T>
struct Formatter<T*> {
    static void print(std::ostream& out, T* p) {
        if (p)
            out << '*' << *p;
        else
            out << "null";
    }
};
template <>
struct Formatter<std::string> {
    static void print(std::ostream& out, const std::string& s) { out << '"' << s << '"'; }
};

template <typename T, std::size_t N>
void print(const char* title, const RingBuffer<T, N>& b) {
    std::cout << title << " [" << b.size() << '/' << b.capacity() << "]:";
    b.forEach([](const T& x) {
        std::cout << ' ';
        Formatter<T>::print(std::cout, x);
    });
    std::cout << '\n';
}

template <typename T>
using Buffer4 = RingBuffer<T, 4>;                         // alias šablon

int main() {
    std::cout << std::boolalpha << "== step 1: buffer, overwriting, copies and moves\n";
    RingBuffer<int, 3> b;
    for (int i = 1; i <= 5; ++i) b.push(i);
    std::cout << "after 1..5:";
    b.forEach([](int v) { std::cout << ' ' << v; });
    std::cout << " (size " << b.size() << '/' << b.capacity() << ")\n";
    std::cout << "full " << b.full() << ", oldest " << b[0] << '\n';
    try {
        b.at(3);
    } catch (const std::out_of_range& e) {
        std::cout << "at(3): " << e.what() << '\n';
    }
    RingBuffer<Tracked, 2> p;
    Tracked x("a");
    p.push(x);                                           // kopija
    p.push(Tracked("b"));                                 // premeštanje privremenog
    p.emplace("c");                                       // prepisuje "a": pravi Tracked, pa ga premesti
    std::cout << "Tracked: copies " << Tracked::copies << ", moves " << Tracked::moves << ", contents "
              << p[0].text << ' ' << p[1].text << '\n';

    std::cout << "== step 2: average via a trait and if constexpr\n";
    RingBuffer<Measurement, 3> m;
    m.emplace(Measurement{"temp", 21.5});
    m.emplace(Measurement{"temp", 22.5});
    std::cout << "average int: " << average(b) << ", average of measurements: " << average(m) << '\n';

    std::cout << "== step 3: variadic and fold\n";
    pushAll(b, 10, 20);
    std::cout << "after pushAll(10, 20):";
    b.forEach([](int v) { std::cout << ' ' << v; });
    std::cout << '\n';
    std::cout << "allInRange(0, 50, 10, 20.5, measurement 21.5): " << allInRange(0, 50, 10, 20.5, m[0])
              << ", with 60: " << allInRange(0, 50, 10, 60) << ", no arguments: " << allInRange(0, 50) << '\n';

    std::cout << "== step 4: CTAD, specializations, alias\n";
    Range o{0.0, 50.0};
    static_assert(std::is_same_v<decltype(o), Range<double>>);
    std::cout << "Range{0.0, 50.0} contains 21.5: " << o.contains(21.5) << ", 60: " << o.contains(60.0) << '\n';
    Buffer4<std::string> names;
    pushAll(names, std::string("temp"), std::string("humidity"));
    names.emplace(3, 'x');
    print("string", names);
    int a = 7, c = 9;
    RingBuffer<int*, 3> ptrs;
    pushAll(ptrs, &a, nullptr, &c);
    print("int*", ptrs);
}
