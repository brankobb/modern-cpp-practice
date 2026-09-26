// Završna vežba dela 5 -- sistem događaja
//   ./build.sh 5-funkcije-kao-vrednosti/zavrsna-vezba/zadatak.cpp
// Uputstvo i spisak lekcija: 5-funkcije-kao-vrednosti/zavrsna-vezba/notes.md
// Rešenje: resenje.cpp (otvori tek kad tvoj izlaz bude isti kao blok
// OČEKIVANI IZLAZ na dnu fajla).
//
// Fajl se kompajlira i ovakav. Piši redom; posle svakog koraka
// otkomentariši njegov deo main()-a.
//
// Korak 1: class Dispecer
//   -- using Id = int; Id pretplati(tema, Rukovalac); bool odjavi(Id);
//   int objavi(tema, poruka) -- pozove rukovaoce te teme redom kojim su
//   se pretplatili i vrati koliko ih je pozvano; brojPretplata().
//   Rukovalac je std::function<void(const Poruka&)> (lekcija 31, sekcija 1).
//   -- u main-u: lambda sa [&brojac] i sa [prefiks] (kopija u trenutku
//   pravljenja) -- lekcija 30, sekcija 5.
// Korak 2: dodajFilter(Filter) -- poruka stiže do rukovalaca samo ako
//   prođe sve filtere; linearno(x, k, n) i primeni(lanac, x) nad
//   std::vector<std::function<double(double)>>. U main-u je prvi korak
//   lanca napravljen sa std::bind, drugi lambdom (lekcija 31, sekcije 3 i 5).
// Korak 3: class Logger -- u konstruktoru se pretplati lambdom koja hvata
//   this, u destruktoru se odjavi (RAII, lekcija 21; capture this:
//   lekcija 30, sekcija 7). Kopiranje zabrani -- zašto?
// Korak 4: rukovalac sme da odjavi sebe (ili drugog) USRED objave.
//   objavi mora to da podnese: obilazi kopiju spiska, a preskače one koji
//   su u međuvremenu odjavljeni (lekcija 27, sekcija 6: invalidacija
//   iteratora).

#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct Poruka {
    std::string izvor;
    double vrednost;
};

using Rukovalac = std::function<void(const Poruka&)>;
using Filter = std::function<bool(const Poruka&)>;

// TODO korak 1, 2, 3, 4

int main() {
    using namespace std::placeholders;

    // Korak 1 -- otkomentariši:
    // std::cout << "== korak 1: pretplata, capture, odjava\n";
    // Dispecer d;
    // int brojac = 0;
    // const auto idBrojaca = d.pretplati("temp", [&brojac](const Poruka&) { ++brojac; });
    // std::string prefiks = "  [temp] ";
    // const auto idIspisa = d.pretplati("temp", [prefiks](const Poruka& p) { std::cout << prefiks << p.vrednost << '\n'; });
    // prefiks = "promenjen ";                          // lambda ima svoju kopiju
    // const int pozvano = d.objavi("temp", {"hala", 21.5});   // pre ispisa: rukovaoci i sami pišu na cout
    // std::cout << "objava temp: " << pozvano << " rukovaoca\n";
    // std::cout << "objava vlaga: " << d.objavi("vlaga", {"hala", 40}) << " rukovaoca\n";
    // d.odjavi(idBrojaca);
    // d.objavi("temp", {"hala", 22.0});
    // std::cout << "brojač " << brojac << " (drugu objavu nije video), pretplata " << d.brojPretplata() << '\n';
    // d.odjavi(idIspisa);

    // Korak 2 -- otkomentariši:
    // std::cout << "== korak 2: filter i lanac obrade (bind i lambda)\n";
    // d.dodajFilter([](const Poruka& p) { return p.vrednost > -50 && p.vrednost < 150; });
    // const std::vector<std::function<double(double)>> uFarenhajt{
    //     std::bind(linearno, _1, 1.8, 32.0),          // isto što i [](double c) { return 1.8 * c + 32; }
    //     [](double f) { return std::round(f * 10) / 10; },
    // };
    // const auto idF =
    //     d.pretplati("temp", [&uFarenhajt](const Poruka& p) { std::cout << "  " << p.vrednost << " C = " << primeni(uFarenhajt, p.vrednost) << " F\n"; });
    // d.objavi("temp", {"hala", 21.5});
    // std::cout << "objava 999 (filter): " << d.objavi("temp", {"hala", 999}) << " rukovaoca\n";
    // d.odjavi(idF);

    // Korak 3 -- otkomentariši:
    // std::cout << "== korak 3: logger se odjavljuje u destruktoru\n";
    // {
    //     Logger log(d, "alarm");
    //     d.objavi("alarm", {"kotao", 91});
    //     d.objavi("alarm", {"kotao", 95});
    //     std::cout << "zapisano " << log.zapisano() << ", pretplata " << d.brojPretplata() << '\n';
    // }
    // std::cout << "posle bloka: pretplata " << d.brojPretplata() << ", objava alarm: " << d.objavi("alarm", {"kotao", 99})
    //           << " rukovaoca\n";

    // Korak 4 -- otkomentariši:
    // std::cout << "== korak 4: rukovalac koji se odjavi tokom objave\n";
    // Dispecer::Id idJednom = 0;
    // idJednom = d.pretplati("start", [&d, &idJednom](const Poruka& p) {
    //     std::cout << "  prvi start od " << p.izvor << ", odjavljujem se\n";
    //     d.odjavi(idJednom);
    // });
    // d.pretplati("start", [](const Poruka& p) { std::cout << "  start od " << p.izvor << '\n'; });
    // d.objavi("start", {"pumpa", 1});
    // d.objavi("start", {"ventil", 1});
    // std::cout << "pretplata na kraju: " << d.brojPretplata() << '\n';
}

/* OČEKIVANI IZLAZ
== korak 1: pretplata, capture, odjava
  [temp] 21.5
objava temp: 2 rukovaoca
objava vlaga: 0 rukovaoca
  [temp] 22
brojač 1 (drugu objavu nije video), pretplata 1
== korak 2: filter i lanac obrade (bind i lambda)
  21.5 C = 70.7 F
objava 999 (filter): 0 rukovaoca
== korak 3: logger se odjavljuje u destruktoru
  log: kotao 91
  log: kotao 95
zapisano 2, pretplata 1
posle bloka: pretplata 0, objava alarm: 0 rukovaoca
== korak 4: rukovalac koji se odjavi tokom objave
  prvi start od pumpa, odjavljujem se
  start od pumpa
  start od ventil
pretplata na kraju: 1
*/
