// Rešenje zadatka ex1_array_variant.

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>

// Korak 1: std::array se prosleđuje kao i svaki objekat (const&), i NE
// raspada se u pokazivač -- veličina je deo tipa.
double average(const std::array<int, 5>& a) {
    int s = 0;
    for (int x : a) s += x;
    return static_cast<double>(s) / static_cast<double>(a.size());
}

// Korak 2: variant zna koji tip trenutno drži. get_if je provera i pristup
// u jednom koraku; std::get<T> bi bacio std::bad_variant_access za pogrešan tip.
using Message = std::variant<int, double, std::string>;

void describe(const Message& m) {
    if (const int* i = std::get_if<int>(&m))
        std::cout << "int " << *i << '\n';
    else if (const double* d = std::get_if<double>(&m))
        std::cout << "double " << *d << '\n';
    else if (const std::string* s = std::get_if<std::string>(&m))
        std::cout << "string " << *s << '\n';
}

// Korak 3: using je čitljiviji od typedef int (*Step)(int); (EMC Item 9).
using Step = int (*)(int);

int twice(int x) { return 2 * x; }
int addOne(int x) { return x + 1; }

// Ime applySteps, a ne apply: argument je std::array, pa bi ADL za
// apply(steps, 10) našao i std::apply, koji bolje odgovara i ne kompajlira se.
int applySteps(const std::array<Step, 2>& steps, int x) {
    for (Step f : steps) x = f(x);
    return x;
}

int main() {
    std::array<int, 5> readings{10, 20, 30, 40, 50};
    std::cout << "count: " << readings.size() << " first: " << readings.front()
              << " last: " << readings.back() << '\n';
    std::cout << "average: " << average(readings) << '\n';
    try {
        std::cout << readings.at(5);
    } catch (const std::out_of_range&) {
        std::cout << "at(5): out_of_range\n";
    }

    for (const Message& m : {Message{42}, Message{3.5}, Message{std::string("hello")}})
        describe(m);

    std::array<Step, 2> steps{twice, addOne};
    std::cout << "10 -> " << applySteps(steps, 10) << '\n';
}
