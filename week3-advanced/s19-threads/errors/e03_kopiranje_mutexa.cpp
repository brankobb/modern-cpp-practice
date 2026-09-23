// EXPECT-GCC: use of deleted function 'Brojac::Brojac(const Brojac&)'
// EXPECT-CLANG: call to implicitly-deleted copy constructor of 'Brojac'
// POGREŠNO: std::mutex se ne kopira ni ne premešta (niti čekaju baš na
// taj objekat, na toj adresi). Zato klasa sa mutex članom nema
// podrazumevani copy konstruktor.
// Ispravno: ne kopiraj takav objekat (prosledi referencu), ili napiši
// copy konstruktor koji zaključa izvor i kopira samo PODATKE, a novi
// objekat dobije svoj mutex.
#include <mutex>
struct Brojac {
    std::mutex m;
    int n = 0;
};
int main() {
    Brojac a;
    Brojac b = a;
    return b.n;
}
