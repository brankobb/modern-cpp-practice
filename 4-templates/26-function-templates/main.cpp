#include <array>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <type_traits>

// Funkcijski šabloni -- ISPRAVNI slučajevi. Sve se kompajlira bez
// upozorenja i radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i
// C++20). Brojevi sekcija prate notes.md.
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira (i jedna greška linkera)
// ./check_cases.sh 4-templates/26-function-templates  proverava ih.

// ---------------------------------------------------------------- 1
// Šablon nije funkcija, nego recept za funkcije. Kompajler napravi
// (instancira) posebnu funkciju za svaki tip sa kojim se pozove.
template <typename T>
T maxOf(T a, T b) {
    return b < a ? a : b;   // traži samo operator< -- sve što ga ima, radi
}

void section1() {
    std::cout << "\n== 1. template and instantiation\n";
    std::cout << "maxOf(3, 7) = " << maxOf(3, 7) << '\n';
    std::cout << "maxOf(2.5, 1.5) = " << maxOf(2.5, 1.5) << '\n';
    std::cout << "maxOf(string) = " << maxOf(std::string("cherry"), std::string("banana")) << '\n';
}

// ---------------------------------------------------------------- 2
void section2() {
    std::cout << "\n== 2. argument deduction\n";
    // maxOf(1, 2.5) se NE kompajlira: T bi bio i int i double (errors/e01).
    // Dedukcija ne radi konverzije -- ali eksplicitan argument ih dozvoljava:
    std::cout << "maxOf<double>(1, 2.5) = " << maxOf<double>(1, 2.5) << '\n';
    // T se dedukuje po pravilima za parametar PO VREDNOSTI (lekcija 10):
    // const i referenca se odbacuju, niz postaje pokazivač.
    const int ci = 4;
    int x = 9;
    int& rx = x;
    static_assert(std::is_same_v<decltype(maxOf(ci, rx)), int>);
    std::cout << "maxOf(const int, int&) returns int: " << maxOf(ci, rx) << '\n';
}

// ---------------------------------------------------------------- 3
// Svaka instancijacija je posebna funkcija -- i ima SVOJU static promenljivu.
template <typename T>
int callCount() {
    static int n = 0;
    return ++n;
}

void section3() {
    std::cout << "\n== 3. each instantiation is a separate function\n";
    callCount<int>();
    callCount<int>();
    std::cout << "callCount<int>: " << callCount<int>() << ", callCount<double>: "
              << callCount<double>() << '\n';
}

// ---------------------------------------------------------------- 4
// Podrazumevani argument šablona + eksplicitni argument.
template <typename Out = double, typename T>
Out average(const T* arr, std::size_t n) {
    Out s{};
    for (std::size_t i = 0; i < n; ++i) s += static_cast<Out>(arr[i]);
    return n ? s / static_cast<Out>(n) : Out{};
}

void section4() {
    std::cout << "\n== 4. explicit and default arguments\n";
    int readings[] = {1, 2, 4};
    std::cout << "average (double): " << average(readings, 3) << '\n';
    std::cout << "average<int>: " << average<int>(readings, 3) << '\n';   // T se i dalje dedukuje
}

// ---------------------------------------------------------------- 5
// C string: opšti maxOf bi poredio ADRESE (zadatak ex2). Overload za
// const char* je ne-šablon, pa pobeđuje kad se tip tačno poklopi.
const char* maxOf(const char* a, const char* b) { return std::strcmp(b, a) < 0 ? a : b; }

// Eksplicitna (potpuna) specijalizacija: telo za JEDAN tip, isti potpis.
template <typename T>
std::string describe(const T&) {
    return "something";
}
template <>
std::string describe<bool>(const bool& b) {
    return b ? "yes" : "no";
}

void section5() {
    std::cout << "\n== 5. overload and explicit specialization\n";
    std::cout << "maxOf(\"cherry\", \"banana\") = " << maxOf("cherry", "banana") << '\n';
    std::cout << "maxOf<>(3, 4) = " << maxOf<>(3, 4) << " (<> requires the template)\n";
    std::cout << "describe(true) = " << describe(true) << ", describe(42) = " << describe(42) << '\n';
}

// ---------------------------------------------------------------- 6
// Ne-tipski parametri: vrednost poznata pri kompajliranju.
template <typename T, std::size_t N>
constexpr std::size_t arraySize(const T (&)[N]) {
    return N;   // N se dedukuje iz tipa niza
}

template <int Min, int Max>
int clampTo(int x) {
    static_assert(Min <= Max, "Min must be <= Max");
    return x < Min ? Min : (x > Max ? Max : x);
}

template <auto V>   // C++17: tip parametra se dedukuje iz vrednosti
constexpr auto valueOf() {
    return V;
}

template <std::size_t N>
std::array<int, N> squares() {
    std::array<int, N> a{};
    for (std::size_t i = 0; i < N; ++i) a[i] = static_cast<int>(i * i);
    return a;
}

void section6() {
    std::cout << "\n== 6. non-type parameters\n";
    int arr[7] = {};
    (void)arr;
    static_assert(arraySize(arr) == 7);
    std::cout << "arraySize(arr) = " << arraySize(arr) << '\n';
    std::cout << "clampTo<0, 100>(150) = " << clampTo<0, 100>(150) << ", (-5) = "
              << clampTo<0, 100>(-5) << '\n';
    std::cout << "valueOf<'x'>() = " << valueOf<'x'>() << ", valueOf<42u>() = " << valueOf<42u>()
              << '\n';
    auto k = squares<5>();
    std::cout << "squares<5>:";
    for (int v : k) std::cout << ' ' << v;
    std::cout << '\n';
}

int main() {
    section1();
    section2();
    section3();
    section4();
    section5();
    section6();
}
