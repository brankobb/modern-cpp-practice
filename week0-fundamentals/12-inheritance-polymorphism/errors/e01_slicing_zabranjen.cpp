// STD: c++17
// EXPECT-GCC: use of deleted function 'SafeAnimal::SafeAnimal(const SafeAnimal&)'
// EXPECT-CLANG: call to deleted constructor of 'SafeAnimal'
// POGREŠNO -- namerno: ovako rešenje za slicing IZGLEDA kad radi.
// SafeAnimal ima obrisan copy konstruktor (C++ Core Guidelines C.67), pa
// prosleđivanje izvedenog objekta PO VREDNOSTI ne može da se kompajlira.
// Bez "= delete" isti kod bi se tiho kompajlirao i odsekao SafeDog deo.
// Ispravno: void describe(const SafeAnimal& a)  -- referenca, bez kopije
#include <string>
class SafeAnimal {
public:
    SafeAnimal() = default;
    SafeAnimal(const SafeAnimal&) = delete;
    SafeAnimal& operator=(const SafeAnimal&) = delete;
    virtual ~SafeAnimal() = default;
    virtual std::string speak() const { return "..."; }
};
class SafeDog : public SafeAnimal {
public:
    std::string speak() const override { return "Av!"; }
};
std::string byValue(SafeAnimal a) { return a.speak(); }
int main() {
    SafeDog d;
    return static_cast<int>(byValue(d).size());
}
