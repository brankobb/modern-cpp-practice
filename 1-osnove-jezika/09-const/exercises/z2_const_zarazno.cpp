// VRSTA: zašto
// DEMO-ERR: NAIVNO discards qualifiers|not marked const
//
// Zadatak 2 -- zašto const metode od samog početka (sekcije 3, 4)
// Rešenje: exercises/solutions/z2_const_zarazno.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-osnove-jezika/09-const/exercises/z2_const_zarazno.cpp -DNAIVNO
//   Klasa Konfiguracija je napisana bez const metoda. Dok je koristiš samo
//   preko običnih objekata, sve radi. Onda neko ispravno napiše funkciju
//   koja prima const Konfiguracija& (da ne kopira i da obeća da ne menja)
//   -- i ništa ne može da pozove. Pročitaj poruku: "passing 'const ...' as
//   'this' argument discards qualifiers" (g++) / "function is not marked
//   const" (clang).
// Korak 2: u #else grani popravi klasu: koje metode treba da budu const, a
//   koje ne? Zatim otkomentariši test.
// Korak 3: zašto rešenje NIJE da ispis() primi Konfiguracija& (bez const)
//   ili Konfiguracija po vrednosti? Upiši odgovor u komentar.

#include <iostream>
#include <map>
#include <string>

#ifdef NAIVNO
class Konfiguracija {
public:
    void postavi(const std::string& k, int v) { m_[k] = v; }
    int uzmi(const std::string& k) { return m_.at(k); }        // nije const
    std::size_t broj() { return m_.size(); }                    // nije const
private:
    std::map<std::string, int> m_;
};

void ispis(const Konfiguracija& c) {
    std::cout << c.broj() << " ključa, baud=" << c.uzmi("baud") << '\n';
}

int main() {
    Konfiguracija c;
    c.postavi("baud", 115200);
    ispis(c);
}
#else
class Konfiguracija {
public:
    // TODO korak 2
};

int main() {
    // Korak 2 -- otkomentariši:
    // Konfiguracija c;
    // c.postavi("baud", 115200);
    // c.postavi("parity", 0);
    // ispis(c);
}
#endif

/* OČEKIVANI IZLAZ
2 ključa, baud=115200
*/
