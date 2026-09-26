// Rešenje zadatka ex1_generic_record.

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

// Korak 1: T se ne vidi iz konstruktora (parametri su It), pa vodič kaže
// da je T tip elementa na koji It pokazuje.
template <typename It>
Channel(std::string, It, It) -> Channel<typename std::iterator_traits<It>::value_type>;

// Korak 2: && fold -- za prazan paket je true. Binarni fold sa 0.0 daje
// double i kad su svi argumenti celi brojevi (average(1, 2) je 1.5, ne 1).
template <typename... T>
bool inRange(double min, double max, T... v) {
    return ((v >= min && v <= max) && ...);
}

template <typename... T>
double average(T... v) {
    return (0.0 + ... + v) / sizeof...(v);
}

// Korak 3: bool pre is_integral (bool jeste integral); poslednja grana u
// else -- inače se instancira i za int (errors/e07).
template <typename T>
std::string formatValue(const T& x) {
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
std::string record(const T&... a) {
    std::string result;
    const char* sep = "";
    ((result += sep + formatValue(a), sep = " | "), ...);
    return result;
}

int main() {
    std::vector<double> temp{21.5, 22.0, 22.5};
    Channel k("temp", temp.begin(), temp.end());
    static_assert(std::is_same_v<decltype(k), Channel<double>>);
    std::cout << k.name() << ": " << k.values().size() << " values\n";

    std::cout << std::boolalpha << "inRange(0, 50, 21.5, 40, 3): " << inRange(0, 50, 21.5, 40, 3)
              << ", inRange(0, 50, 21.5, 90): " << inRange(0, 50, 21.5, 90) << '\n';
    std::cout << "average(1, 2, 4.5): " << average(1, 2, 4.5) << '\n';

    std::cout << record(true, 42, 21.456, "temp", std::string("ok"), temp) << '\n';
}
