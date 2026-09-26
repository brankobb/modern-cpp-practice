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
struct Sensor {
    Sensor(std::string i, int k) : name(std::move(i)), channel(k) {}
    std::string name;
    int channel;
};

template <typename T>
class Registry {
public:
    template <typename... Args>
    T& create(Args&&... args) {
        items_.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return *items_.back();
    }
    std::size_t count() const { return items_.size(); }

private:
    std::vector<std::unique_ptr<T>> items_;
};

void section1() {
    std::cout << "\n== 1. perfect forwarding + variadic\n";
    Registry<Sensor> r;
    std::string name = "temperature";
    Sensor& a = r.create(name, 1);                  // lvalue: kopira se
    Sensor& b = r.create(std::string("pressure"), 2);   // rvalue: pomera se
    std::cout << a.name << '/' << a.channel << ", " << b.name << '/' << b.channel
              << ", caller's name still: " << name << ", total: " << r.count() << '\n';
}

// ---------------------------------------------------------------- 2
// Variadic: rekurzija (C++11 stil) -- osnovni slučaj + "prvi i ostali".
void printRec() { std::cout << '\n'; }
template <typename First, typename... Rest>
void printRec(const First& p, const Rest&... rest) {
    std::cout << p << (sizeof...(rest) ? ", " : "");
    printRec(rest...);
}

// C++17 fold izrazi -- bez rekurzije.
template <typename... Args>
auto sum(Args... args) {
    return (0 + ... + args);          // binarni fold sa početnom vrednošću: radi i za 0 argumenata
}
template <typename... Args>
bool allPositive(Args... args) {
    return (... && (args > 0));       // unarni fold; za prazan pack && daje true
}
template <typename... Args>
void printFold(const Args&... args) {
    const char* sep = "";
    ((std::cout << sep << args, sep = ", "), ...);   // fold preko zareza
    std::cout << '\n';
}

int square(int x) { return x * x; }
template <typename... Args>
int sumOfSquares(Args... args) {
    return sum(square(args)...);    // obrazac se proširi: square(a1), square(a2), ...
}

void section2() {
    std::cout << "\n== 2. variadic templates\n";
    std::cout << "recursion: ";
    printRec(1, 2.5, "three", 'c');
    std::cout << "fold: ";
    printFold(1, 2.5, "three", 'c');
    std::cout << "sum(1, 2, 3, 4) = " << sum(1, 2, 3, 4) << ", sum() = " << sum() << '\n';
    std::cout << "allPositive(3, 5, -1) = " << allPositive(3, 5, -1) << ", allPositive() = "
              << allPositive() << '\n';
    std::cout << "sumOfSquares(1, 2, 3) = " << sumOfSquares(1, 2, 3) << '\n';
}

// ---------------------------------------------------------------- 3
// Klasni šablon: stek fiksnog kapaciteta, bez heap-a (embedded).
template <typename T, std::size_t N>
class Stack {
public:
    void push(const T& v) {
        if (size_ == N) throw std::overflow_error("Stack is full");
        data_[size_++] = v;
    }
    T pop();                          // definicija van klase, ispod
    std::size_t size() const { return size_; }
    static constexpr std::size_t capacity() { return N; }

    // Instancira se tek kad se POZOVE -- Stack<T> za T bez operator< je
    // ispravan dok se largest() ne koristi (errors/e04).
    T largest() const {
        T m = data_[0];
        for (std::size_t i = 1; i < size_; ++i)
            if (m < data_[i]) m = data_[i];
        return m;
    }

private:
    std::array<T, N> data_{};
    std::size_t size_ = 0;
};

template <typename T, std::size_t N>   // van klase: ponovi parametre i Stack<T, N>::
T Stack<T, N>::pop() {
    if (size_ == 0) throw std::underflow_error("Stack is empty");
    return data_[--size_];
}

struct Point {
    int x = 0, y = 0;
};   // nema operator<

// CTAD (C++17): argumenti klasnog šablona iz konstruktora.
template <typename T>
struct Wrapper {
    explicit Wrapper(T v) : value(std::move(v)) {}
    T value;
};
Wrapper(const char*)->Wrapper<std::string>;   // deduction guide: literal -> std::string

void section3() {
    std::cout << "\n== 3. class templates\n";
    Stack<int, 4> s;
    s.push(3);
    s.push(9);
    s.push(5);
    std::cout << "size " << s.size() << "/" << Stack<int, 4>::capacity() << ", largest "
              << s.largest() << ", pop " << s.pop() << '\n';

    Stack<Point, 2> t;                 // bez operator< -- u redu, largest() se ne zove
    t.push({1, 2});
    std::cout << "Stack<Point>: pop -> (" << t.pop().x << ", ...)\n";

    std::pair p{1, 2.5};              // std::pair<int, double>
    std::array a{1, 2, 3};            // std::array<int, 3>
    Wrapper o{"text"};                // Wrapper<std::string>, zbog deduction guide-a
    static_assert(std::is_same_v<decltype(p), std::pair<int, double>>);
    static_assert(std::is_same_v<decltype(a), std::array<int, 3>>);
    static_assert(std::is_same_v<decltype(o), Wrapper<std::string>>);
    std::cout << "CTAD: pair " << p.first << '/' << p.second << ", array " << a.size()
              << ", Wrapper<std::string> length " << o.value.size() << '\n';
}

// ---------------------------------------------------------------- 4
// Eksplicitna (potpuna) specijalizacija klasnog šablona.
template <typename T>
struct TypeName {
    static std::string name() { return "something"; }
};
template <>
struct TypeName<bool> {                   // potpuno drugačija klasa za bool
    static std::string name() { return "bool"; }
};
template <>
struct TypeName<int> {
    static std::string name() { return "int"; }
};

// Specijalizacija samo jedne metode, ostatak klase ostaje opšti.
template <typename T>
struct Box {
    T v;
    std::string show() const { return std::to_string(v); }
    bool empty() const { return false; }
};
template <>
std::string Box<std::string>::show() const {
    return '"' + v + '"';
}

void section4() {
    std::cout << "\n== 4. explicit specialization of a class\n";
    std::cout << "TypeName<bool>: " << TypeName<bool>::name() << ", TypeName<int>: " << TypeName<int>::name()
              << ", TypeName<double>: " << TypeName<double>::name() << '\n';
    Box<int> k1{42};
    Box<std::string> k2{"hello"};
    std::cout << "Box<int>: " << k1.show() << ", Box<std::string>: " << k2.show()
              << ", empty: " << k2.empty() << '\n';
}

// ---------------------------------------------------------------- 5
// Delimična specijalizacija: za CELU FAMILIJU tipova.
template <typename T>
struct TypeName<T*> {
    static std::string name() { return "pointer to " + TypeName<T>::name(); }
};
template <typename T>
struct TypeName<std::vector<T>> {
    static std::string name() { return "vector of " + TypeName<T>::name(); }
};
template <typename T, std::size_t N>
struct TypeName<T[N]> {
    static std::string name() { return "array of " + std::to_string(N) + " x " + TypeName<T>::name(); }
};

void section5() {
    std::cout << "\n== 5. partial specialization\n";
    std::cout << TypeName<int*>::name() << '\n';
    std::cout << TypeName<std::vector<bool*>>::name() << '\n';
    std::cout << TypeName<int[4]>::name() << '\n';
    std::cout << TypeName<double**>::name() << '\n';
}

// ---------------------------------------------------------------- 6
// Alias šabloni (C++11): typedef ne može da ima parametre, using može.
template <std::size_t N>
using Bytes = std::array<std::uint8_t, N>;
template <typename T>
using ByName = std::map<std::string, T>;
using Packet = Bytes<8>;             // obični alias: isto što i typedef

void section6() {
    std::cout << "\n== 6. alias templates\n";
    Packet p{0x01, 0x02};
    ByName<int> channels{{"temp", 1}, {"pressure", 2}};
    static_assert(std::is_same_v<Packet, std::array<std::uint8_t, 8>>);   // alias NIJE nov tip
    std::cout << "Packet: " << p.size() << " bytes, first " << int(p[0]) << "; pressure channel: "
              << channels["pressure"] << '\n';
}

// ---------------------------------------------------------------- 7
// Type traits: pitanja o tipu (i transformacije) pri kompajliranju.
// Sopstveni trait: primarni šablon "ne", delimična specijalizacija "da".
template <typename T>
struct isVector : std::false_type {};
template <typename T>
struct isVector<std::vector<T>> : std::true_type {};
template <typename T>
inline constexpr bool isVector_v = isVector<T>::value;   // _v pomoćnik (variable template)

template <typename T>
std::string sizeOrValue(const T& x) {
    if constexpr (isVector_v<T>)
        return "vector, " + std::to_string(x.size()) + " elem.";
    else if constexpr (std::is_arithmetic_v<T>)
        return "number " + std::to_string(x);
    else
        return "something else";
}

void section7() {
    std::cout << "\n== 7. type traits\n";
    static_assert(std::is_integral_v<int> && !std::is_integral_v<double>);
    static_assert(std::is_same_v<std::remove_reference_t<const int&>, const int>);
    static_assert(std::is_same_v<std::remove_cv_t<std::remove_reference_t<const int&>>, int>);
    static_assert(std::is_same_v<std::decay_t<const int&>, int>);
    static_assert(std::is_same_v<std::decay_t<int[3]>, int*>);
    static_assert(std::is_same_v<std::conditional_t<(sizeof(int) >= 4), int, long>, int>);
    static_assert(std::is_same_v<std::common_type_t<int, double>, double>);
    static_assert(isVector_v<std::vector<int>> && !isVector_v<int>);
    std::cout << sizeOrValue(std::vector<int>{1, 2, 3}) << "; " << sizeOrValue(7)
              << "; " << sizeOrValue(std::string("x")) << '\n';
}

// ---------------------------------------------------------------- 8
// static_assert u šablonu: jasna poruka na mestu upotrebe, umesto greške
// duboko u telu (errors/e03).
template <typename T>
class Measurement {
    static_assert(std::is_arithmetic_v<T>, "Measurement<T>: T must be an arithmetic type");

public:
    explicit Measurement(T v) : v_(v) {}
    T value() const { return v_; }

private:
    T v_;
};

void section8() {
    std::cout << "\n== 8. static_assert\n";
    Measurement<double> m(21.5);
    static_assert(sizeof(Measurement<std::uint16_t>) == 2);   // C++17: poruka nije obavezna
    std::cout << "Measurement<double>: " << m.value() << '\n';
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
}
