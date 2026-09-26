// KIND: usage
//
// Zadatak 1 -- deduction guide, fold izrazi i if constexpr u jednom
// generičkom zapisu merenja (sekcije 1, 2, 3, 5)
//   ./build.sh 4-templates/29-cpp17-templates/exercises/ex1_generican_zapis.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_generican_zapis.cpp
//
// Korak 1: napiši deduction guide za Kanal tako da
//   Kanal k("temp", temp.begin(), temp.end());  bude Kanal<double>.
//   (Zašto ga kompajler ne zaključi sam?)
// Korak 2: bool uOpsegu(double min, double max, T... v) -- da li su SVE
//   vrednosti u [min, max]; jedan fold, bez petlje.
//   double prosek(T... v) -- fold sa početnom vrednošću, pa sizeof...(v).
// Korak 3: std::string formatiraj(const T& x) sa if constexpr:
//   bool -> "ON"/"OFF", ceo broj -> kako jeste, realan -> 1 decimala
//   (std::fixed, std::setprecision(1)), nešto što se konvertuje u
//   std::string -> pod navodnicima, sve ostalo (kontejner) -> "[size]".
//   std::string zapis(const T&... a) -- formatiraj svaki, spoji sa " | "
//   (fold po zarezu, kao ispisi u main.cpp, sekcija 3).

#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

template <typename T>
class Kanal {
public:
    template <typename It>
    Kanal(std::string ime, It prvi, It poslednji) : ime_(std::move(ime)), vrednosti_(prvi, poslednji) {}
    const std::string& ime() const { return ime_; }
    const std::vector<T>& vrednosti() const { return vrednosti_; }

private:
    std::string ime_;
    std::vector<T> vrednosti_;
};

// TODO korak 1, 2, 3

int main() {
    std::vector<double> temp{21.5, 22.0, 22.5};

    // Korak 1 -- otkomentariši:
    // Kanal k("temp", temp.begin(), temp.end());
    // static_assert(std::is_same_v<decltype(k), Kanal<double>>);
    // std::cout << k.ime() << ": " << k.vrednosti().size() << " vrednosti\n";

    // Korak 2 -- otkomentariši:
    // std::cout << std::boolalpha << "uOpsegu(0, 50, 21.5, 40, 3): " << uOpsegu(0, 50, 21.5, 40, 3)
    //           << ", uOpsegu(0, 50, 21.5, 90): " << uOpsegu(0, 50, 21.5, 90) << '\n';
    // std::cout << "prosek(1, 2, 4.5): " << prosek(1, 2, 4.5) << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << zapis(true, 42, 21.456, "temp", std::string("ok"), temp) << '\n';
}

/* EXPECTED OUTPUT
temp: 3 vrednosti
uOpsegu(0, 50, 21.5, 40, 3): true, uOpsegu(0, 50, 21.5, 90): false
prosek(1, 2, 4.5): 2.5
ON | 42 | 21.5 | "temp" | "ok" | [3]
*/
