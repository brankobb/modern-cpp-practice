// Rešenje zadatka ex3_any_literal.

#include <any>
#include <iostream>
#include <map>
#include <string>

using Properties = std::map<std::string, std::any>;

// Ako se upiše "temp" (nije dobro): u any ode const char*, a any_cast<std::string>
// traži baš std::string -- bad_any_cast.
// Treba ovako: napravi std::string pre upisa; tada je tip u any-ju tačno onaj koji se čita.
void setName(Properties& s) {
    using namespace std::string_literals;
    s["name"] = "temp"s;
}

int main() {
    Properties s;
    s["id"] = 7;
    setName(s);
    std::cout << "id: " << std::any_cast<int>(s["id"]) << '\n';
    try {
        std::cout << "name: " << std::any_cast<std::string>(s.at("name")) << '\n';
    } catch (const std::bad_any_cast&) {
        bool isPointer = s.at("name").type() == typeid(const char*);
        std::cout << "wrong type" << (isPointer ? " (const char*)" : "") << '\n';
    } catch (const std::out_of_range&) {
        std::cout << "not set\n";
    }
}
