// EXPECT-UB: SEGV on unknown address
// POGREŠNO: malloc za strukturu sa std::string, pa dodela članu.
// Zašto: malloc samo rezerviše bajtove; konstruktor std::string se nikad ne
//   pozove, pa u memoriji nema ispravnog string objekta. u->name = "Ana"
//   poziva operator= na tom smeću, koji čita neispravan pokazivač na podatke.
//   ASan puni malloc memoriju bajtom 0xbe, pa pad bude pouzdan; bez njega
//   ishod zavisi od toga šta je ranije bilo u memoriji.
// Ispravno: new User{"Ana", 1} (poziva konstruktor), ili placement new na
//   malloc memoriji + ručni poziv destruktora (main.cpp, sekcija 2).
#include <cstdio>
#include <cstdlib>
#include <string>

struct User {
    std::string name;
    int id;
};

int main() {
    User* u = static_cast<User*>(std::malloc(sizeof(User)));
    u->name = "Ana";
    u->id = 1;
    std::printf("%s\n", u->name.c_str());
    std::free(u);
}
