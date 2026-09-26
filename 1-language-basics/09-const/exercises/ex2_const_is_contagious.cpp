// KIND: why
// DEMO-ERR: NAIVE discards qualifiers|not marked const
//
// Zadatak 2 -- zašto const metode od samog početka (sekcije 3, 4)
// Rešenje: exercises/solutions/ex2_const_is_contagious.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/09-const/exercises/ex2_const_is_contagious.cpp -DNAIVE
//   Klasa Config je napisana bez const metoda. Dok je koristiš samo
//   preko običnih objekata, sve radi. Onda neko ispravno napiše funkciju
//   koja prima const Config& (da ne kopira i da obeća da ne menja)
//   -- i ništa ne može da pozove. Pročitaj poruku: "passing 'const ...' as
//   'this' argument discards qualifiers" (g++) / "function is not marked
//   const" (clang).
// Korak 2: u #else grani popravi klasu: koje metode treba da budu const, a
//   koje ne? Zatim otkomentariši test.
// Korak 3: zašto rešenje NIJE da print() primi Config& (bez const)
//   ili Config po vrednosti? Upiši odgovor u komentar.

#include <iostream>
#include <map>
#include <string>

#ifdef NAIVE
class Config {
public:
    void set(const std::string& k, int v) { m_[k] = v; }
    int get(const std::string& k) { return m_.at(k); }         // nije const
    std::size_t count() { return m_.size(); }                   // nije const
private:
    std::map<std::string, int> m_;
};

void print(const Config& c) {
    std::cout << c.count() << " keys, baud=" << c.get("baud") << '\n';
}

int main() {
    Config c;
    c.set("baud", 115200);
    print(c);
}
#else
class Config {
public:
    // TODO korak 2
};

int main() {
    // Korak 2 -- otkomentariši:
    // Config c;
    // c.set("baud", 115200);
    // c.set("parity", 0);
    // print(c);
}
#endif

/* EXPECTED OUTPUT
2 keys, baud=115200
*/
