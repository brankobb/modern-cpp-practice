// EXPECT-UB: heap-use-after-free
// UB: string_view ne produžava život onoga u šta gleda. Privremeni
// std::string nestane na kraju iskaza; sv posle toga pokazuje u
// oslobođenu memoriju. (Tekst je duži od 15 znakova, pa je na heap-u;
// kraći bi bio u SSO baferu na steku, i ASan bi javio stack-use-after-scope.)
// Ispravno: sačuvaj string (std::string s = ...; std::string_view sv = s;),
// ili neka promenljiva bude std::string.
#include <iostream>
#include <string>
#include <string_view>
int main() {
    std::string_view sv = std::string("temperature sensor in hall no. 3");
    std::cout << sv << '\n';
}
