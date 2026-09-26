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
// ./check_cases.sh 1-osnove-jezika/01-tipovi-ulaz-izlaz-funkcije  proverava oba.
// Pre pokretanja: za svaku sekciju probaj da predvidiš ispis, PA pokreni.

// ---------------------------------------------------------------- 1
void sekcija1() {
    std::cout << "\n== 1. veličine (ovaj sistem)\n";
    std::cout << "sizeof char/short/int/long/long long/pokazivač: " << sizeof(char) << ' '
              << sizeof(short) << ' ' << sizeof(int) << ' ' << sizeof(long) << ' '
              << sizeof(long long) << ' ' << sizeof(void*) << '\n';
    // Standard garantuje samo minimum (int >= 16 bita, long >= 32...).
    // Kad je širina bitna, tip to kaže sam:
    static_assert(sizeof(std::int32_t) == 4);
    static_assert(sizeof(std::uint64_t) == 8);
    std::cout << "int opseg: " << std::numeric_limits<int>::min() << " .. "
              << std::numeric_limits<int>::max() << '\n';
    std::cout << "uint8_t opseg: " << +std::numeric_limits<std::uint8_t>::min() << " .. "
              << +std::numeric_limits<std::uint8_t>::max() << '\n';
}

// ---------------------------------------------------------------- 2
void sekcija2() {
    std::cout << "\n== 2. char i bajtovi\n";
    // Da li je char signed zavisi od platforme (x86: da, ARM: obično ne).
    std::cout << "char je signed: " << std::boolalpha << std::numeric_limits<char>::is_signed
              << std::noboolalpha << '\n';
    // Ako bajt držiš u običnom char-u (nije dobro): 200 postane -56 na x86.
    char c = static_cast<char>(200);
    std::cout << "char(200) kao int: " << int(c) << '\n';
    // Treba ovako: unsigned char / uint8_t za bajtove -- 200 ostaje 200.
    std::uint8_t b = 200;
    // uint8_t je unsigned char, pa ga << ispiše kao ZNAK. + ga promoviše u int.
    std::uint8_t slovo = 65;
    std::cout << "uint8_t 200: " << +b << ", uint8_t 65 kao znak: " << slovo
              << ", kao broj: " << +slovo << '\n';
}

// ---------------------------------------------------------------- 3
void sekcija3() {
    std::cout << "\n== 3. promocije i konverzije\n";
    unsigned char a = 200, b = 100;
    auto s = a + b;   // promocija: oba u int pre sabiranja
    static_assert(std::is_same_v<decltype(s), int>);
    std::cout << "unsigned char 200 + 100 = " << s << " (tip int)\n";

    // -1 < 0u: -1 se konvertuje u unsigned. Ovde to pišemo EKSPLICITNO, da
    // se vidi šta kompajler radi (implicitni oblik daje -Wsign-compare).
    std::cout << "static_cast<unsigned>(-1) = " << static_cast<unsigned>(-1)
              << ", pa je (-1 < 0u): " << (static_cast<unsigned>(-1) < 0u) << '\n';

    // Treba ovako: poredi u istom, širem signed tipu kad unsigned vrednost
    // staje u njega...
    int temp = -5;
    unsigned prag = 10;
    std::cout << "-5 < 10u, kao long long: " << (static_cast<long long>(temp) < prag) << '\n';
#if __cplusplus >= 202002L
    // ...ili, od C++20, std::cmp_less -- poredi matematički tačno.
    static_assert(std::cmp_less(-1, 0u));
#endif
}

// ---------------------------------------------------------------- 4
void sekcija4() {
    std::cout << "\n== 4. prekoračenje\n";
    // Unsigned: definisan, po modulu 2^n.
    unsigned umax = std::numeric_limits<unsigned>::max();
    std::cout << "UINT_MAX + 1u = " << umax + 1u << '\n';
    // Signed: UB (ub/u01). Treba ovako: proveri PRE operacije.
    int x = std::numeric_limits<int>::max();
    if (x > std::numeric_limits<int>::max() - 1)
        std::cout << "INT_MAX + 1 bi prekoračio -- ne računam\n";
    // ...ili računaj u širem tipu.
    long long sire = static_cast<long long>(x) + 1;
    std::cout << "INT_MAX + 1 u long long = " << sire << '\n';
    // Pomeranje: samo za manje od širine tipa (ub/u03).
    std::cout << "1u << 31 = " << (1u << 31) << '\n';
}

// ---------------------------------------------------------------- 5
void sekcija5() {
    std::cout << "\n== 5. deljenje\n";
    std::cout << "7 / 2 = " << 7 / 2 << ", -7 / 2 = " << -7 / 2 << ", -7 % 2 = " << -7 % 2
              << " (ka nuli)\n";
    int zbir = 7, n = 2;
    // Ako pišeš static_cast<double>(zbir / n) (nije dobro): deljenje je već
    // celobrojno. Treba ovako: cast PRE deljenja.
    std::cout << "prosek: " << static_cast<double>(zbir) / n << '\n';
    // Deljenje nulom je UB (ub/u04) -- proveri delilac.
    int delilac = 0;
    if (delilac != 0)
        std::cout << zbir / delilac << '\n';
    else
        std::cout << "delilac je 0 -- ne delim\n";
}

// ---------------------------------------------------------------- 6
void sekcija6() {
    std::cout << "\n== 6. pokretni zarez\n";
    double a = 0.1 + 0.2;
    std::cout << std::setprecision(17) << "0.1 + 0.2 = " << a << std::setprecision(6) << '\n';
    std::cout << "== 0.3: " << (a == 0.3) << ", |razlika| < 1e-9: " << (std::abs(a - 0.3) < 1e-9)
              << '\n';
    double nan = std::numeric_limits<double>::quiet_NaN();
    double isti = nan;
    // NaN nije jednak ničemu, ni samom sebi (IEEE 754) -- zato std::isnan.
    std::cout << "nan == nan: " << (nan == isti) << ", isnan(nan): " << std::isnan(nan) << '\n';
}

// ---------------------------------------------------------------- 7
void sekcija7() {
    std::cout << "\n== 7. ulaz i stanje stream-a\n";
    std::istringstream in("42 abc 7");
    int x = -1, y = -1, z = -1;
    in >> x >> y;   // "abc" nije broj
    std::cout << "x=" << x << ", y=" << y << " (0 posle neuspeha), fail=" << in.fail() << '\n';
    in >> z;        // stream je u fail stanju: ne radi ništa
    std::cout << "z bez clear(): " << z << '\n';
    // Treba ovako: clear() skine failbit, ignore() preskoči loš tekst.
    in.clear();
    in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    in >> z;
    std::cout << "z posle clear()+ignore(): " << z << '\n';

    std::istringstream veliki("99999999999");
    int w = 0;
    veliki >> w;
    std::cout << "\"99999999999\" u int: " << w << ", fail=" << veliki.fail() << '\n';

    // Čitaj dok uspeva, a greške preskači.
    std::istringstream brojevi("1 2 x 3 y 4");
    int zbir = 0, gresaka = 0, v = 0;
    while (true) {
        if (brojevi >> v) {
            zbir += v;
        } else if (brojevi.eof()) {
            break;
        } else {
            ++gresaka;
            brojevi.clear();
            brojevi.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
        }
    }
    std::cout << "zbir=" << zbir << ", preskočeno=" << gresaka << '\n';

    // >> pa getline: posle >> u baferu ostaje '\n'. std::ws ga preskoči.
    std::istringstream unos("3\nMarko Marković\n");
    int broj = 0;
    std::string ime;
    unos >> broj;
    std::getline(unos >> std::ws, ime);
    std::cout << "broj=" << broj << ", ime=[" << ime << "]\n";
}

// ---------------------------------------------------------------- 8
void sekcija8() {
    std::cout << "\n== 8. manipulatori\n";
    std::cout << '[' << std::setw(5) << 42 << "] [" << 42 << "] (setw važi samo jednom)\n";
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
[[nodiscard]] int ocena(int poeni);

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

void sekcija9() {
    std::cout << "\n== 9. funkcije\n";
    std::cout << "ocena(95)=" << ocena(95) << ", ocena(55)=" << ocena(55) << ", ocena(20)="
              << ocena(20) << '\n';
    auto [lo, hi] = minMax(4, -2, 9);
    std::cout << "minMax(4, -2, 9): " << lo << ' ' << hi << '\n';
    // Redosled računanja argumenata nije određen (g++ i clang se razlikuju),
    // pa argumenti ne smeju da zavise jedan od drugog. Ovde su nezavisni.
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

// Svaka grana vraća vrednost -- izlazak sa kraja bez return-a je UB (ub/u06).
// Redosled provera: od najveće granice naniže.
int ocena(int poeni) {
    if (poeni >= 90) return 10;
    if (poeni >= 50) return 6;
    return 5;
}
