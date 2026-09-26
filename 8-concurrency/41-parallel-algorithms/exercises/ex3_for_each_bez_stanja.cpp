// KIND: why
// DEMO-ERR: NAIVE incomplete
//
// Zadatak 3 -- zašto paralelni for_each ne vraća funktor (sekcija 4; errors/e01)
// Rešenje: exercises/solutions/ex3_for_each_bez_stanja.cpp
//
// Brojač alarma je funktor koji broji merenja iznad praga. Sekvencijalno
// radi: for_each vrati funktor, a u njemu je nakupljen broj.
//
// Korak 1: pokušaj da prevedeš naivnu (paralelnu) verziju:
//     ./build.sh 8-concurrency/41-parallel-algorithms/exercises/ex3_for_each_bez_stanja.cpp -DNAIVE
//   Greška: rezultat for_each(par, ...) je void. U paralelnom izvršavanju
//   svaka nit dobije svoju KOPIJU funktora i broji svoj deo -- ne postoji
//   jedan brojač koji bi mogao da se vrati. (Sa referencom na zajednički
//   brojač bio bi data race, notes.md, sekcija 3.)
// Korak 2: u #else grani napiši alarmi() paralelno, bez funktora sa
//   stanjem: algoritam koji sam broji.

#include <algorithm>
#include <execution>
#include <iostream>
#include <vector>

struct BrojacAlarma {
    double prag;
    long n = 0;
    void operator()(double x) {
        if (x > prag) ++n;
    }
};

#ifdef NAIVE
long alarmi(const std::vector<double>& v, double prag) {
    auto b = std::for_each(std::execution::par, v.begin(), v.end(), BrojacAlarma{prag});
    return b.n;
}
#else
// Sekvencijalno radi -- for_each bez politike vrati funktor:
long alarmiSekvencijalno(const std::vector<double>& v, double prag) {
    return std::for_each(v.begin(), v.end(), BrojacAlarma{prag}).n;
}
// TODO korak 2 (dok ne napišeš, vraća 0)
long alarmi(const std::vector<double>&, double) { return 0; }
#endif

int main() {
    std::vector<double> v(10000);
    for (std::size_t i = 0; i < v.size(); ++i) v[i] = static_cast<double>(i % 100);   // 0..99
    std::cout << "iznad 90: " << alarmi(v, 90.0) << '\n';
}

/* EXPECTED OUTPUT
iznad 90: 900
*/
