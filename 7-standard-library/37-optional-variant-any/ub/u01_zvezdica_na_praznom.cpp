// EXPECT-UB: Assertion 'this->_M_is_engaged\(\)' failed
// FLAGS: -D_GLIBCXX_ASSERTIONS
// UB: *r i r-> NE proveravaju da li optional ima vrednost. Na praznom
// čitaju memoriju u kojoj nije napravljen nijedan objekat.
// ASan ovo NE vidi: memorija pripada optional-u, pa program tiho ispiše
// neku vrednost (ovde 0). -D_GLIBCXX_ASSERTIONS uključuje provere u
// libstdc++ (kao u lekciji 07, sekcija 7), i one ga uhvate.
// Ispravno: if (r) ... *r; ili r.value() (baci bad_optional_access), ili
// r.value_or(podrazumevano).
#include <iostream>
#include <optional>
#include <string>
std::optional<int> nadji(const std::string& ime) {
    if (ime == "temp") return 21;
    return std::nullopt;
}
int main() {
    auto r = nadji("vlaga");
    std::cout << *r << '\n';
}
