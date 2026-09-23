// Rešenje zadatka z3_vector_zagrade.

#include <iostream>
#include <string>
#include <vector>

// Ako radiš "std::vector<int>{n, vrednost}" (nije dobro): {} prvo traži
// initializer_list konstruktor, a n i vrednost su int, pa dobiješ DVA
// elementa [n vrednost]. Za "n kopija" treba ovako, sa (), da se pozove
// vector(size_type, const T&):
std::vector<int> napuni(int n, int vrednost) {
    return std::vector<int>(static_cast<std::size_t>(n), vrednost);
}

void ispisi(const char* opis, const std::vector<int>& v) {
    std::cout << opis << ": [";
    for (std::size_t i = 0; i < v.size(); ++i) std::cout << (i ? " " : "") << v[i];
    std::cout << "]\n";
}

int main() {
    std::vector<int> v1(10);           // 10 nula: vector(size_type)
    std::vector<int> v2{10};           // 1 element: initializer_list<int>
    std::vector<int> v3(10, 20);       // 10 x 20
    std::vector<int> v4{10, 20};       // 2 elementa: 10 i 20
    // 10 nije konvertibilno u std::string, pa initializer_list konstruktor
    // otpada i {} se vraća na vector(size_type): 10 praznih stringova.
    std::vector<std::string> s1{10};
    std::vector<std::string> s2{"10"}; // 1 string "10"
    std::cout << "v1(10): " << v1.size() << '\n'
              << "v2{10}: " << v2.size() << '\n'
              << "v3(10, 20): " << v3.size() << '\n'
              << "v4{10, 20}: " << v4.size() << '\n'
              << "s1{10}: " << s1.size() << '\n'
              << "s2{\"10\"}: " << s2.size() << '\n';

    ispisi("napuni(3, 7)", napuni(3, 7));
}
