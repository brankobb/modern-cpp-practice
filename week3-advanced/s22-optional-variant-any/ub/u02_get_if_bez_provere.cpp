// EXPECT-UB: member call on null pointer of type '(struct string|std::basic_string<char>)'
// UB: get_if vraća nullptr kad variant ne drži traženi tip -- to je
// njegova svrha. Ovde se rezultat koristi bez provere.
// Ispravno: if (auto* s = std::get_if<std::string>(&v)) { ... s->size() ... }
#include <iostream>
#include <string>
#include <variant>
int main() {
    std::variant<int, std::string> v = 5;
    std::string* s = std::get_if<std::string>(&v);
    std::cout << s->size() << '\n';
}
