// KIND: usage
//
// Zadatak 1 -- deduction guide, fold izrazi i if constexpr u jednom
// generičkom zapisu merenja (sekcije 1, 2, 3, 5)
//   ./build.sh 4-templates/29-cpp17-templates/exercises/ex1_generic_record.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_generic_record.cpp
//
// Korak 1: napiši deduction guide za Channel tako da
//   Channel k("temp", temp.begin(), temp.end());  bude Channel<double>.
//   (Zašto ga kompajler ne zaključi sam?)
// Korak 2: bool inRange(double min, double max, T... v) -- da li su SVE
//   vrednosti u [min, max]; jedan fold, bez petlje.
//   double average(T... v) -- fold sa početnom vrednošću, pa sizeof...(v).
// Korak 3: std::string formatValue(const T& x) sa if constexpr:
//   bool -> "ON"/"OFF", ceo broj -> kako jeste, realan -> 1 decimala
//   (std::fixed, std::setprecision(1)), nešto što se konvertuje u
//   std::string -> pod navodnicima, sve ostalo (kontejner) -> "[size]".
//   std::string record(const T&... a) -- formatValue za svaki, spoji sa " | "
//   (fold po zarezu, kao print u main.cpp, sekcija 3).

#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

template <typename T>
class Channel {
public:
    template <typename It>
    Channel(std::string n, It first, It last) : name_(std::move(n)), values_(first, last) {}
    const std::string& name() const { return name_; }
    const std::vector<T>& values() const { return values_; }

private:
    std::string name_;
    std::vector<T> values_;
};

// TODO korak 1, 2, 3

int main() {
    std::vector<double> temp{21.5, 22.0, 22.5};

    // Korak 1 -- otkomentariši:
    // Channel k("temp", temp.begin(), temp.end());
    // static_assert(std::is_same_v<decltype(k), Channel<double>>);
    // std::cout << k.name() << ": " << k.values().size() << " values\n";

    // Korak 2 -- otkomentariši:
    // std::cout << std::boolalpha << "inRange(0, 50, 21.5, 40, 3): " << inRange(0, 50, 21.5, 40, 3)
    //           << ", inRange(0, 50, 21.5, 90): " << inRange(0, 50, 21.5, 90) << '\n';
    // std::cout << "average(1, 2, 4.5): " << average(1, 2, 4.5) << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << record(true, 42, 21.456, "temp", std::string("ok"), temp) << '\n';
}

/* EXPECTED OUTPUT
temp: 3 values
inRange(0, 50, 21.5, 40, 3): true, inRange(0, 50, 21.5, 90): false
average(1, 2, 4.5): 2.5
ON | 42 | 21.5 | "temp" | "ok" | [3]
*/
