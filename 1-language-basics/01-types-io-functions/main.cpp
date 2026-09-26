#include <climits>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

// Primitivni tipovi, ulaz/izlaz, funkcije -- ISPRAVNI slučajevi. Sve se
// kompajlira bez upozorenja i radi bez ASan/UBSan prijava (g++ 13 i
// clang 18, C++17 i C++20). Brojevi sekcija prate notes.md.
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 1-language-basics/01-types-io-functions  proverava oba.
// Pre pokretanja: za svaku sekciju probaj da predvidiš ispis, PA pokreni.

// ---------------------------------------------------------------- 1
void section1() {
    std::cout << "\n== 1. sizes (this system)\n";
    std::cout << "sizeof char/short/int/long/long long/pointer: " << sizeof(char) << ' '
              << sizeof(short) << ' ' << sizeof(int) << ' ' << sizeof(long) << ' '
              << sizeof(long long) << ' ' << sizeof(void*) << '\n';
    // Standard garantuje samo minimum (int >= 16 bita, long >= 32...).
    // Kad je širina bitna, tip to kaže sam:
    static_assert(sizeof(std::int32_t) == 4);
    static_assert(sizeof(std::uint64_t) == 8);
    std::cout << "int range: " << std::numeric_limits<int>::min() << " .. "
              << std::numeric_limits<int>::max() << '\n';
    std::cout << "uint8_t range: " << +std::numeric_limits<std::uint8_t>::min() << " .. "
              << +std::numeric_limits<std::uint8_t>::max() << '\n';
}

// ---------------------------------------------------------------- 2
void section2() {
    std::cout << "\n== 2. char and bytes\n";
    // Da li je char signed zavisi od platforme (x86: da, ARM: obično ne).
    std::cout << "char is signed: " << std::boolalpha << std::numeric_limits<char>::is_signed
              << std::noboolalpha << '\n';
    // Ako bajt držiš u običnom char-u (nije dobro): 200 postane -56 na x86.
    char c = static_cast<char>(200);
    std::cout << "char(200) as int: " << int(c) << '\n';
    // Treba ovako: unsigned char / uint8_t za bajtove -- 200 ostaje 200.
    std::uint8_t b = 200;
    // uint8_t je unsigned char, pa ga << ispiše kao ZNAK. + ga promoviše u int.
    std::uint8_t letter = 65;
    std::cout << "uint8_t 200: " << +b << ", uint8_t 65 as a character: " << letter
              << ", as a number: " << +letter << '\n';
}

// ---------------------------------------------------------------- 3
void section3() {
    std::cout << "\n== 3. promotions and conversions\n";
    unsigned char a = 200, b = 100;
    auto s = a + b;   // promocija: oba u int pre sabiranja
    static_assert(std::is_same_v<decltype(s), int>);
    std::cout << "unsigned char 200 + 100 = " << s << " (type int)\n";

    // -1 < 0u: -1 se konvertuje u unsigned. Ovde to pišemo EKSPLICITNO, da
    // se vidi šta kompajler radi (implicitni oblik daje -Wsign-compare).
    std::cout << "static_cast<unsigned>(-1) = " << static_cast<unsigned>(-1)
              << ", so (-1 < 0u): " << (static_cast<unsigned>(-1) < 0u) << '\n';

    // Treba ovako: poredi u istom, širem signed tipu kad unsigned vrednost
    // staje u njega...
    int temp = -5;
    unsigned threshold = 10;
    std::cout << "-5 < 10u, as long long: " << (static_cast<long long>(temp) < threshold) << '\n';
#if __cplusplus >= 202002L
    // ...ili, od C++20, std::cmp_less -- poredi matematički tačno.
    static_assert(std::cmp_less(-1, 0u));
#endif
}

// ---------------------------------------------------------------- 4
void section4() {
    std::cout << "\n== 4. overflow\n";
    // Unsigned: definisan, po modulu 2^n.
    unsigned umax = std::numeric_limits<unsigned>::max();
    std::cout << "UINT_MAX + 1u = " << umax + 1u << '\n';
    // Signed: UB (ub/u01). Treba ovako: proveri PRE operacije.
    int x = std::numeric_limits<int>::max();
    if (x > std::numeric_limits<int>::max() - 1)
        std::cout << "INT_MAX + 1 would overflow -- not computing it\n";
    // ...ili računaj u širem tipu.
    long long wider = static_cast<long long>(x) + 1;
    std::cout << "INT_MAX + 1 in long long = " << wider << '\n';
    // Pomeranje: samo za manje od širine tipa (ub/u03).
    std::cout << "1u << 31 = " << (1u << 31) << '\n';
}

// ---------------------------------------------------------------- 5
void section5() {
    std::cout << "\n== 5. division\n";
    std::cout << "7 / 2 = " << 7 / 2 << ", -7 / 2 = " << -7 / 2 << ", -7 % 2 = " << -7 % 2
              << " (toward zero)\n";
    int sum = 7, n = 2;
    // Ako pišeš static_cast<double>(sum / n) (nije dobro): deljenje je već
    // celobrojno. Treba ovako: cast PRE deljenja.
    std::cout << "average: " << static_cast<double>(sum) / n << '\n';
    // Deljenje nulom je UB (ub/u04) -- proveri delilac.
    int divisor = 0;
    if (divisor != 0)
        std::cout << sum / divisor << '\n';
    else
        std::cout << "divisor is 0 -- not dividing\n";
}

// ---------------------------------------------------------------- 6
void section6() {
    std::cout << "\n== 6. floating point\n";
    double a = 0.1 + 0.2;
    std::cout << std::setprecision(17) << "0.1 + 0.2 = " << a << std::setprecision(6) << '\n';
    std::cout << "== 0.3: " << (a == 0.3) << ", |difference| < 1e-9: " << (std::abs(a - 0.3) < 1e-9)
              << '\n';
    double nan = std::numeric_limits<double>::quiet_NaN();
    double same = nan;
    // NaN nije jednak ničemu, ni samom sebi (IEEE 754) -- zato std::isnan.
    std::cout << "nan == nan: " << (nan == same) << ", isnan(nan): " << std::isnan(nan) << '\n';
}

// ---------------------------------------------------------------- 7
void section7() {
    std::cout << "\n== 7. input and stream state\n";
    std::istringstream in("42 abc 7");
    int x = -1, y = -1, z = -1;
    in >> x >> y;   // "abc" nije broj
    std::cout << "x=" << x << ", y=" << y << " (0 after failure), fail=" << in.fail() << '\n';
    in >> z;        // stream je u fail stanju: ne radi ništa
    std::cout << "z without clear(): " << z << '\n';
    // Treba ovako: clear() skine failbit, ignore() preskoči loš tekst.
    in.clear();
    in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    in >> z;
    std::cout << "z after clear()+ignore(): " << z << '\n';

    std::istringstream big("99999999999");
    int w = 0;
    big >> w;
    std::cout << "\"99999999999\" into int: " << w << ", fail=" << big.fail() << '\n';

    // Čitaj dok uspeva, a greške preskači.
    std::istringstream numbers("1 2 x 3 y 4");
    int sum = 0, errors = 0, v = 0;
    while (true) {
        if (numbers >> v) {
            sum += v;
        } else if (numbers.eof()) {
            break;
        } else {
            ++errors;
            numbers.clear();
            numbers.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
        }
    }
    std::cout << "sum=" << sum << ", skipped=" << errors << '\n';

    // >> pa getline: posle >> u baferu ostaje '\n'. std::ws ga preskoči.
    std::istringstream input("3\nJohn Smith\n");
    int number = 0;
    std::string name;
    input >> number;
    std::getline(input >> std::ws, name);
    std::cout << "number=" << number << ", name=[" << name << "]\n";
}

// ---------------------------------------------------------------- 8
void section8() {
    std::cout << "\n== 8. manipulators\n";
    std::cout << '[' << std::setw(5) << 42 << "] [" << 42 << "] (setw applies only once)\n";
    std::cout << std::fixed << std::setprecision(2) << 3.14159 << '\n';
    std::cout.unsetf(std::ios::floatfield);   // vrati podrazumevani format
    std::cout << std::setprecision(6);
    std::cout << std::setfill('0') << std::setw(3) << 7 << std::setfill(' ') << '\n';
    std::cout << std::hex << 255 << ' ' << 16;
    // hex je "lepljiv": bez std::dec bi i SVI sledeći int-ovi bili hex.
    std::cout << std::dec << ' ' << 16 << '\n';
}

// ---------------------------------------------------------------- 9
// Deklaracija pre upotrebe (definicija je ispod main-a).
[[nodiscard]] int grade(int points);

struct MinMax {
    int min;
    int max;
};

// F.20: više rezultata -> struct, ne izlazni parametri.
MinMax minMax(int a, int b, int c) {
    int lo = a, hi = a;
    for (int v : {b, c}) {
        if (v < lo) lo = v;
        if (v > hi) hi = v;
    }
    return {lo, hi};
}

void section9() {
    std::cout << "\n== 9. functions\n";
    std::cout << "grade(95)=" << grade(95) << ", grade(55)=" << grade(55) << ", grade(20)="
              << grade(20) << '\n';
    auto [lo, hi] = minMax(4, -2, 9);
    std::cout << "minMax(4, -2, 9): " << lo << ' ' << hi << '\n';
    // Redosled računanja argumenata nije određen (g++ i clang se razlikuju),
    // pa argumenti ne smeju da zavise jedan od drugog. Ovde su nezavisni.
}

int main() {
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

// Svaka grana vraća vrednost -- izlazak sa kraja bez return-a je UB (ub/u06).
// Redosled provera: od najveće granice naniže.
int grade(int points) {
    if (points >= 90) return 10;
    if (points >= 50) return 6;
    return 5;
}
