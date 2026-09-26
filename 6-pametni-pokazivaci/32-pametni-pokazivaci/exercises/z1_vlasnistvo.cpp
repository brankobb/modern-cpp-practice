// VRSTA: upotreba
//
// Zadatak 1 -- unique_ptr, shared_ptr i weak_ptr po nameni (sekcije 1-4)
//   ./build.sh 6-pametni-pokazivaci/32-pametni-pokazivaci/exercises/z1_vlasnistvo.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_vlasnistvo.cpp
//
// Korak 1: jedan vlasnik -> unique_ptr.
//   std::unique_ptr<Senzor> napraviSenzor(int id) -- fabrika
//   (std::make_unique). class Sistem sa std::vector<std::unique_ptr<Senzor>>
//   i void dodaj(std::unique_ptr<Senzor> s) -- "sink" po vrednosti:
//   pozivalac mora da napiše std::move, pa se u kodu VIDI da predaje
//   vlasništvo. std::size_t broj() const.
// Korak 2: više vlasnika -> shared_ptr. Dva modula (struct Modul sa
//   std::shared_ptr<const Konfig> cfg) dele istu konfiguraciju
//   (std::make_shared). use_count() pokazuje broj vlasnika.
// Korak 3: posmatrač bez vlasništva -> weak_ptr. struct Posmatrac sa
//   std::weak_ptr<const Konfig> cfg i metodom void proveri() const: lock()
//   vrati shared_ptr (pun ako objekat još živi, prazan ako ne).

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct Senzor {
    explicit Senzor(int senzorId) : id(senzorId) { std::cout << "Senzor(" << id << ")\n"; }
    ~Senzor() { std::cout << "~Senzor(" << id << ")\n"; }
    int id;
};

struct Konfig {
    explicit Konfig(std::string i) : ime(std::move(i)) {}
    std::string ime;
    ~Konfig() { std::cout << "~Konfig(" << ime << ")\n"; }
};

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // {
    //     Sistem sis;
    //     auto s = napraviSenzor(1);
    //     sis.dodaj(std::move(s));
    //     sis.dodaj(napraviSenzor(2));
    //     std::cout << "s posle predaje: " << (s ? "pun" : "prazan") << ", u sistemu: " << sis.broj() << '\n';
    // }

    // Korak 2 i 3 -- otkomentariši:
    // Posmatrac pos;
    // {
    //     auto cfg = std::make_shared<const Konfig>("v1");
    //     Modul a{cfg}, b{cfg};
    //     pos.cfg = cfg;
    //     std::cout << "vlasnika: " << cfg.use_count() << '\n';
    //     pos.proveri();
    // }
    // pos.proveri();
}

/* OČEKIVANI IZLAZ
Senzor(1)
Senzor(2)
s posle predaje: prazan, u sistemu: 2
~Senzor(1)
~Senzor(2)
vlasnika: 3
posmatrač vidi: v1
~Konfig(v1)
posmatrač vidi: ništa (objekat uništen)
*/
