// STD: c++17
// EXPECT-GCC: use of deleted function 'Holder::Holder(const Holder&)'
// EXPECT-CLANG: call to implicitly-deleted copy constructor of 'Holder'
// POGREŠNO: "Holder(const Holder&) = default;" za klasu sa unique_ptr članom.
// Zašto: = default traži od kompajlera da napiše funkciju "kao i inače".
//   Kad to ne može (unique_ptr nema kopiju), funkcija je definisana kao
//   OBRISANA ([dcl.fct.def.default]); greška je tek na mestu upotrebe.
// Ispravno: napiši kopiju sam (duboka kopija objekta), ili prihvati da je
//   klasa move-only i ne deklariši kopiju.
#include <memory>

struct Holder {
    std::unique_ptr<int> p;
    Holder() = default;
    Holder(const Holder&) = default;
};

int main() {
    Holder a;
    Holder b = a;
    (void)b;
}
