// KIND: usage
//
// Zadatak 1 -- std::array, std::variant i alias za pokazivač na funkciju
// (sekcije 3, 6, 7, 8)
//   ./build.sh 1-language-basics/05-compound-types/exercises/ex1_array_variant.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_array_variant.cpp
//
// Korak 1: double average(const std::array<int, 5>& a) -- prosek elemenata.
//   U testu: size(), front(), back(), i at(5) koji baca
//   std::out_of_range (za razliku od [5], koji je UB).
// Korak 2: using Message = std::variant<int, double, std::string>;
//   void describe(const Message& m) ispisuje "int 42" / "double 3.5" /
//   "string hello". Koristi std::get_if (vraća nullptr kad variant ne
//   drži taj tip).
// Korak 3: using Step = int (*)(int); -- alias za pokazivač na funkciju.
//   Napiši int twice(int) i int addOne(int), i
//   int applySteps(const std::array<Step, 2>& steps, int x) koja redom
//   primeni korake.

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // std::array<int, 5> readings{10, 20, 30, 40, 50};
    // std::cout << "count: " << readings.size() << " first: " << readings.front()
    //           << " last: " << readings.back() << '\n';
    // std::cout << "average: " << average(readings) << '\n';
    // try {
    //     std::cout << readings.at(5);
    // } catch (const std::out_of_range&) {
    //     std::cout << "at(5): out_of_range\n";
    // }

    // Korak 2 -- otkomentariši:
    // for (const Message& m : {Message{42}, Message{3.5}, Message{std::string("hello")}})
    //     describe(m);

    // Korak 3 -- otkomentariši:
    // std::array<Step, 2> steps{twice, addOne};
    // std::cout << "10 -> " << applySteps(steps, 10) << '\n';
}

/* EXPECTED OUTPUT
count: 5 first: 10 last: 50
average: 30
at(5): out_of_range
int 42
double 3.5
string hello
10 -> 21
*/
