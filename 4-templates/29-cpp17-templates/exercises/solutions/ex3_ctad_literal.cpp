// Rešenje zadatka ex3_ctad_literal.

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <utility>

// Ako se tabela napravi od std::pair{"temp", 21} (nije dobro): CTAD da
// const char*, i mapa poredi adrese umesto teksta.
// Treba ovako: literal sa sufiksom s je std::string, pa je i ključ string.
auto napraviTabelu() {
    using namespace std::string_literals;
    return std::map{std::pair{"temp"s, 21}, std::pair{"vlaga"s, 40}};
}

int main() {
    auto tabela = napraviTabelu();
    char ulaz[16];
    for (const char* ime : {"temp", "vlaga"}) {
        std::strcpy(ulaz, ime);                  // ime stiglo "spolja", u bafer
        auto it = tabela.find(ulaz);
        std::cout << ulaz << ": ";
        if (it == tabela.end())
            std::cout << "nije pronađen\n";
        else
            std::cout << it->second << '\n';
    }
}
