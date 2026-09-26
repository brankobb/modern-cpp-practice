// Rešenje zadatka ex3_any_literal.

#include <any>
#include <iostream>
#include <map>
#include <string>

using Svojstva = std::map<std::string, std::any>;

// Ako se upiše "temp" (nije dobro): u any ode const char*, a any_cast<std::string>
// traži baš std::string -- bad_any_cast.
// Treba ovako: napravi std::string pre upisa; tada je tip u any-ju tačno onaj koji se čita.
void postaviIme(Svojstva& s) {
    using namespace std::string_literals;
    s["ime"] = "temp"s;
}

int main() {
    Svojstva s;
    s["id"] = 7;
    postaviIme(s);
    std::cout << "id: " << std::any_cast<int>(s["id"]) << '\n';
    try {
        std::cout << "ime: " << std::any_cast<std::string>(s.at("ime")) << '\n';
    } catch (const std::bad_any_cast&) {
        bool pokazivac = s.at("ime").type() == typeid(const char*);
        std::cout << "pogrešan tip" << (pokazivac ? " (const char*)" : "") << '\n';
    } catch (const std::out_of_range&) {
        std::cout << "nije postavljeno\n";
    }
}
