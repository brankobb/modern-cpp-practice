// STD: c++17
// EXPECT-GCC: no matching function for call to 'std::__cxx11::basic_string<char>::basic_string(Person&)'
// EXPECT-CLANG: no matching constructor for initialization of 'std::string'
// POGREŠNO: šablonski konstruktor sa T&& uz copy konstruktor, pa kopija
//   ne-const objekta.
// Zašto: za Person b(p) (p nije const) šablon sa T = Person& je TAČAN match,
//   a copy konstruktor traži dodavanje const. Pobedi šablon (EMC Item 26),
//   i pokuša da napravi std::string od Person-a. Greška se vidi samo zato što
//   string ne zna šta da radi sa Person; da šablon radi nešto drugo, kopija
//   bi tiho radila pogrešnu stvar (main.cpp, sekcija 5).
// Ispravno: ograniči šablon (C++20: requires !std::is_same_v<std::decay_t<T>,
//   Person>; C++17: std::enable_if_t), ili umesto šablona konkretni
//   konstruktori (Person(std::string name)).
#include <string>
#include <utility>

struct Person {
    std::string name;
    Person() = default;
    Person(const Person&) = default;
    template <typename T>
    explicit Person(T&& n) : name(std::forward<T>(n)) {}
};

int main() {
    Person p;
    Person b(p);
    (void)b;
}
