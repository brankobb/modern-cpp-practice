// Rešenje zadatka z1_generican_zapis.

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

// Korak 1: T se ne vidi iz konstruktora (parametri su It), pa vodič kaže
// da je T tip elementa na koji It pokazuje.
template <typename It>
Kanal(std::string, It, It) -> Kanal<typename std::iterator_traits<It>::value_type>;

// Korak 2: && fold -- za prazan paket je true. Binarni fold sa 0.0 daje
// double i kad su svi argumenti celi brojevi (prosek(1, 2) je 1.5, ne 1).
template <typename... T>
bool uOpsegu(double min, double max, T... v) {
    return ((v >= min && v <= max) && ...);
}

template <typename... T>
double prosek(T... v) {
    return (0.0 + ... + v) / sizeof...(v);
}

// Korak 3: bool pre is_integral (bool jeste integral); poslednja grana u
// else -- inače se instancira i za int (errors/e07).
template <typename T>
std::string formatiraj(const T& x) {
    std::ostringstream out;
    if constexpr (std::is_same_v<T, bool>)
        out << (x ? "ON" : "OFF");
    else if constexpr (std::is_integral_v<T>)
        out << x;
    else if constexpr (std::is_floating_point_v<T>)
        out << std::fixed << std::setprecision(1) << x;
    else if constexpr (std::is_convertible_v<T, std::string>)
        out << '"' << std::string(x) << '"';
    else
        out << '[' << x.size() << ']';
    return out.str();
}

template <typename... T>
std::string zapis(const T&... a) {
    std::string rez;
    const char* sep = "";
    ((rez += sep + formatiraj(a), sep = " | "), ...);
    return rez;
}

int main() {
    std::vector<double> temp{21.5, 22.0, 22.5};
    Kanal k("temp", temp.begin(), temp.end());
    static_assert(std::is_same_v<decltype(k), Kanal<double>>);
    std::cout << k.ime() << ": " << k.vrednosti().size() << " vrednosti\n";

    std::cout << std::boolalpha << "uOpsegu(0, 50, 21.5, 40, 3): " << uOpsegu(0, 50, 21.5, 40, 3)
              << ", uOpsegu(0, 50, 21.5, 90): " << uOpsegu(0, 50, 21.5, 90) << '\n';
    std::cout << "prosek(1, 2, 4.5): " << prosek(1, 2, 4.5) << '\n';

    std::cout << zapis(true, 42, 21.456, "temp", std::string("ok"), temp) << '\n';
}
