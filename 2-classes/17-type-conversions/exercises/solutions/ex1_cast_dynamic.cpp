// Rešenje zadatka ex1_cast_dynamic.

#include <iostream>
#include <string>
#include <utility>
#include <vector>

// Korak 1: cast na JEDAN operand pre deljenja, pa je deljenje u double.
// static_cast<double>(sum / n) bi bilo kasno -- celobrojno deljenje je već
// odseklo ostatak.
double average(const std::vector<int>& v) {
    if (v.empty()) return 0;
    int sum = 0;
    for (int x : v) sum += x;
    return static_cast<double>(sum) / static_cast<double>(v.size());
}

// Korak 2: dynamic_cast radi samo za polimorfne tipove (errors/e03), i za
// pokazivač vraća nullptr kad objekat nije tog tipa (za referencu baca
// std::bad_cast).
struct Message {
    virtual ~Message() = default;
};
struct Text : Message {
    explicit Text(std::string s) : content(std::move(s)) {}
    std::string content;
};
struct Command : Message {
    explicit Command(int c) : code(c) {}
    int code;
};

void handle(const Message& m) {
    if (const auto* c = dynamic_cast<const Command*>(&m))
        std::cout << "command " << c->code << '\n';
    else
        std::cout << "not a command\n";
}

// Korak 3: konverzija na jednom mestu (konstruktor odredišta), obe
// explicit -- nijedna se ne dešava tiho.
struct Fahrenheit {
    double f;
};

class Celsius {
public:
    explicit Celsius(Fahrenheit t) : c_((t.f - 32.0) * 5.0 / 9.0) {}
    explicit operator double() const { return c_; }

private:
    double c_;
};

int main() {
    std::cout << "average {3, 4}: " << average({3, 4}) << '\n';
    std::cout << "average {}: " << average({}) << '\n';

    Text t("hello");
    Command c(7);
    handle(t);
    handle(c);

    Celsius deg(Fahrenheit{212});
    std::cout << "212 F = " << static_cast<double>(deg) << " C\n";
}
