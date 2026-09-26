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
T maks(T a, T b) {
    return b < a ? a : b;   // traži samo operator< -- sve što ga ima, radi
}

void sekcija1() {
    std::cout << "\n== 1. šablon i instancijacija\n";
    std::cout << "maks(3, 7) = " << maks(3, 7) << '\n';
    std::cout << "maks(2.5, 1.5) = " << maks(2.5, 1.5) << '\n';
    std::cout << "maks(string) = " << maks(std::string("jabuka"), std::string("banana")) << '\n';
}

// ---------------------------------------------------------------- 2
void sekcija2() {
    std::cout << "\n== 2. dedukcija argumenata\n";
    // maks(1, 2.5) se NE kompajlira: T bi bio i int i double (errors/e01).
    // Dedukcija ne radi konverzije -- ali eksplicitan argument ih dozvoljava:
    std::cout << "maks<double>(1, 2.5) = " << maks<double>(1, 2.5) << '\n';
    // T se dedukuje po pravilima za parametar PO VREDNOSTI (lekcija 10):
    // const i referenca se odbacuju, niz postaje pokazivač.
    const int ci = 4;
    int x = 9;
    int& rx = x;
    static_assert(std::is_same_v<decltype(maks(ci, rx)), int>);
    std::cout << "maks(const int, int&) vraća int: " << maks(ci, rx) << '\n';
}

// ---------------------------------------------------------------- 3
// Svaka instancijacija je posebna funkcija -- i ima SVOJU static promenljivu.
template <typename T>
int brojPoziva() {
    static int n = 0;
    return ++n;
}

void sekcija3() {
    std::cout << "\n== 3. svaka instancijacija je posebna funkcija\n";
    brojPoziva<int>();
    brojPoziva<int>();
    std::cout << "brojPoziva<int>: " << brojPoziva<int>() << ", brojPoziva<double>: "
              << brojPoziva<double>() << '\n';
}

// ---------------------------------------------------------------- 4
// Podrazumevani argument šablona + eksplicitni argument.
template <typename Izlaz = double, typename T>
Izlaz prosek(const T* niz, std::size_t n) {
    Izlaz s{};
    for (std::size_t i = 0; i < n; ++i) s += static_cast<Izlaz>(niz[i]);
    return n ? s / static_cast<Izlaz>(n) : Izlaz{};
}

void sekcija4() {
    std::cout << "\n== 4. eksplicitni i podrazumevani argumenti\n";
    int ocitavanja[] = {1, 2, 4};
    std::cout << "prosek (double): " << prosek(ocitavanja, 3) << '\n';
    std::cout << "prosek<int>: " << prosek<int>(ocitavanja, 3) << '\n';   // T se i dalje dedukuje
}

// ---------------------------------------------------------------- 5
// C string: opšti maks bi poredio ADRESE (zadatak ex2). Overload za
// const char* je ne-šablon, pa pobeđuje kad se tip tačno poklopi.
const char* maks(const char* a, const char* b) { return std::strcmp(b, a) < 0 ? a : b; }

// Eksplicitna (potpuna) specijalizacija: telo za JEDAN tip, isti potpis.
template <typename T>
std::string opisi(const T&) {
    return "nešto";
}
template <>
std::string opisi<bool>(const bool& b) {
    return b ? "da" : "ne";
}

void sekcija5() {
    std::cout << "\n== 5. overload i eksplicitna specijalizacija\n";
    std::cout << "maks(\"jabuka\", \"banana\") = " << maks("jabuka", "banana") << '\n';
    std::cout << "maks<>(3, 4) = " << maks<>(3, 4) << " (<> traži šablon)\n";
    std::cout << "opisi(true) = " << opisi(true) << ", opisi(42) = " << opisi(42) << '\n';
}

// ---------------------------------------------------------------- 6
// Ne-tipski parametri: vrednost poznata pri kompajliranju.
template <typename T, std::size_t N>
constexpr std::size_t velicina(const T (&)[N]) {
    return N;   // N se dedukuje iz tipa niza
}

template <int Min, int Max>
int ogranici(int x) {
    static_assert(Min <= Max, "Min mora biti <= Max");
    return x < Min ? Min : (x > Max ? Max : x);
}

template <auto V>   // C++17: tip parametra se dedukuje iz vrednosti
constexpr auto vrednost() {
    return V;
}

template <std::size_t N>
std::array<int, N> kvadrati() {
    std::array<int, N> a{};
    for (std::size_t i = 0; i < N; ++i) a[i] = static_cast<int>(i * i);
    return a;
}

void sekcija6() {
    std::cout << "\n== 6. ne-tipski parametri\n";
    int niz[7] = {};
    (void)niz;
    static_assert(velicina(niz) == 7);
    std::cout << "velicina(niz) = " << velicina(niz) << '\n';
    std::cout << "ogranici<0, 100>(150) = " << ogranici<0, 100>(150) << ", (-5) = "
              << ogranici<0, 100>(-5) << '\n';
    std::cout << "vrednost<'x'>() = " << vrednost<'x'>() << ", vrednost<42u>() = " << vrednost<42u>()
              << '\n';
    auto k = kvadrati<5>();
    std::cout << "kvadrati<5>:";
    for (int v : k) std::cout << ' ' << v;
    std::cout << '\n';
}

int main() {
    sekcija1();
    sekcija2();
    sekcija3();
    sekcija4();
    sekcija5();
    sekcija6();
}
