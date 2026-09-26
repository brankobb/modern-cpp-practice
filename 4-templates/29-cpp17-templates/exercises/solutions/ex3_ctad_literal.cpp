// Rešenje zadatka ex3_ctad_literal.

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <utility>

// Ako se tabela napravi od std::pair{"temp", 21} (nije dobro): CTAD da
// const char*, i mapa poredi adrese umesto teksta.
// Treba ovako: literal sa sufiksom s je std::string, pa je i ključ string.
auto makeTable() {
    using namespace std::string_literals;
    return std::map{std::pair{"temp"s, 21}, std::pair{"humidity"s, 40}};
}

int main() {
    auto table = makeTable();
    char input[16];
    for (const char* name : {"temp", "humidity"}) {
        std::strcpy(input, name);                  // ime stiglo "spolja", u bafer
        auto it = table.find(input);
        std::cout << input << ": ";
        if (it == table.end())
            std::cout << "not found\n";
        else
            std::cout << it->second << '\n';
    }
}
