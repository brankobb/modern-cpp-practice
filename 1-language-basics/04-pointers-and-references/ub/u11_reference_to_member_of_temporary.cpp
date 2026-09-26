// EXPECT-UB: heap-use-after-free
// UB: getter vraća referencu na ČLAN, a objekat je privremen -- nestaje na
// kraju izraza, pa referenca visi. Getter je potpuno ispravan; greška je u
// načinu poziva. (Ime je dugačko da bi string bio na heap-u.)
// Ispravno: std::string name = makePerson().getName();  -- kopija
//       ili: Person p = makePerson(); const std::string& name = p.getName();
#include <iostream>
#include <string>
struct Person {
    std::string name;
    const std::string& getName() const { return name; }
};
Person makePerson() {
    return Person{"Alexander Alexandrovich Peterson"};
}
int main() {
    const std::string& name = makePerson().getName();
    std::cout << name << "\n";
}
