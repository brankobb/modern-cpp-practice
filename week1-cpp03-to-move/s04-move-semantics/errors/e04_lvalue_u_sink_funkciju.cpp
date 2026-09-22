// STD: c++17
// EXPECT-GCC: cannot bind rvalue reference of type 'std::string&&'
// EXPECT-CLANG: no matching function for call to 'store'
// POGREŠNO: funkcija prima std::string&& ("uzimam tvoj string"), a
//   pozvana je sa promenljivom.
// Zašto: isto kao e01: parametar T&& prima samo rvalue. Pozivalac mora
//   svesno da preda objekat: store(v, std::move(s)).
// Ispravno: store(v, std::move(s)); ili parametar po vrednosti
//   (std::string value), pa pozivalac bira: store(v, s) kopira,
//   store(v, std::move(s)) pomera (week2 s07).
#include <string>
#include <utility>
#include <vector>

void store(std::vector<std::string>& out, std::string&& value) {
    out.push_back(std::move(value));
}

int main() {
    std::vector<std::string> v;
    std::string s = "x";
    store(v, s);
}
