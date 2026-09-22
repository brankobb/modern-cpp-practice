// STD: c++17
// EXPECT-GCC: 'int config::value' conflicts with a previous declaration
// EXPECT-CLANG: target of using declaration conflicts with declaration already in scope
// POGREŠNO: using-deklaracija za ime koje već postoji u istom scope-u.
// Zašto: using-deklaracija se ponaša kao DEKLARACIJA u tom scope-u. Lokalna
//   promenljiva value i config::value bi bile dve različite stvari pod istim
//   imenom u istom bloku. Sa using-DIREKTIVOM (using namespace config;) ista
//   lokalna promenljiva bi tiho sakrila config::value (main.cpp, sekcija 2).
// Ispravno: drugo ime za lokalnu promenljivu, ili config::value bez using-a.
namespace config { int value = 1; }

int main() {
    int value = 2;
    using config::value;
    return value;
}
