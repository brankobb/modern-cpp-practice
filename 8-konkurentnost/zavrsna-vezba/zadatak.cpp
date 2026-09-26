// Završna vežba dela 8 -- pipeline merenja
// SANITIZER: thread
//   ./build.sh 8-konkurentnost/zavrsna-vezba/zadatak.cpp
//   ./build.sh 8-konkurentnost/zavrsna-vezba/zadatak.cpp --tsan
// Uputstvo, spisak lekcija i nova tema (std::condition_variable):
// 8-konkurentnost/zavrsna-vezba/notes.md
// Rešenje: resenje.cpp (otvori tek kad tvoj izlaz bude isti kao blok
// OČEKIVANI IZLAZ na dnu fajla).
//
// Fajl se kompajlira i ovakav. Piši redom; posle svakog koraka
// otkomentariši njegov deo main()-a. Pokreni i sa --tsan: ThreadSanitizer
// ne sme ništa da prijavi, ni posle više pokretanja.
//
// Korak 1: template <typename T> class BezbedanRed (notes.md, "Nova tema")
//   -- članovi: std::mutex, std::condition_variable, std::deque<T>,
//   bool zatvoren_.
//   -- void posalji(T x): pod lock_guard-om dodaj na kraj; ako je red
//   zatvoren, baci std::logic_error("slanje u zatvoren red"); notify_one
//   posle otključavanja (lekcija 39, sekcije 5 i 6).
//   -- std::optional<T> primi(): unique_lock i wait SA PREDIKATOM (ima
//   nešto ili je zatvoren); prazan optional znači "zatvoren i prazan, nema
//   više posla" (lekcija 37, sekcija 1).
//   -- void zatvori(): postavi zatvoren_ pod mutex-om, pa notify_all.
// Korak 2: proizvođač i potrošač
//   -- int proizvodjac(BezbedanRed<Merenje>&, int senzor, int n,
//   int otkazPosle = -1): šalje {senzor, senzor * 100 + i} za i = 0..n-1;
//   kad je i == otkazPosle, baci std::runtime_error("senzor S: nema
//   odgovora"); vraća n.
//   -- std::map<int, Zbir> potrosac(BezbedanRed<Merenje>&): prima dok
//   primi() ne vrati prazan optional; svaki potrošač ima SVOJU mapu, bez
//   deljenja i bez mutex-a (lekcija 39, sekcije 2 i 4).
// Korak 3: Rezultat pokreni(int potrosaca, senzori, int otkazujeSenzor,
// int otkazPosle)
//   -- potrošači i proizvođači preko std::async(std::launch::async, ...),
//   red se prosleđuje sa std::ref (lekcija 40, sekcija 1).
//   -- class ZatvoriNaKraju: RAII, zatvara red u destruktoru (lekcija 21,
//   sekcija 1); drži ga u bloku oko proizvođača, pa se red zatvori čim su
//   svi proizvođači gotovi -- i kad neki baci.
//   -- izuzetak proizvođača stiže kroz get(): uhvati ga i dodaj poruku u
//   greske (lekcija 40, sekcija 6); rezultate potrošača spoji sa spoji().
// Korak 4: isto, ali senzor 2 otkaže posle 50 merenja -- ostali se
// obrade do kraja, a program se ne zaglavi.

#include <condition_variable>
#include <deque>
#include <future>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

struct Merenje {
    int senzor;
    long vrednost;
};

struct Zbir {
    int n = 0;
    long suma = 0;
};

void spoji(std::map<int, Zbir>& ukupno, const std::map<int, Zbir>& deo) {
    for (const auto& [senzor, z] : deo) {
        ukupno[senzor].n += z.n;
        ukupno[senzor].suma += z.suma;
    }
}

struct Rezultat {
    std::map<int, Zbir> poSenzoru;
    std::vector<std::string> greske;
};

void ispisi(const Rezultat& r) {
    int ukupno = 0;
    for (const auto& [senzor, z] : r.poSenzoru) {
        std::cout << "  senzor " << senzor << ": " << z.n << " merenja, suma " << z.suma << '\n';
        ukupno += z.n;
    }
    std::cout << "  ukupno " << ukupno << " merenja";
    for (const auto& g : r.greske) std::cout << "; greška: " << g;
    std::cout << '\n';
}

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // std::cout << std::boolalpha << "== korak 1: red u jednoj niti\n";
    // BezbedanRed<Merenje> red;
    // red.posalji({1, 10});
    // red.posalji({2, 20});
    // red.zatvori();
    // const auto a = red.primi();
    // const auto b = red.primi();
    // const auto c = red.primi();
    // std::cout << "primljeno " << a->vrednost << ", " << b->vrednost << ", posle zatvaranja prazan: " << !c.has_value()
    //           << '\n';
    // try {
    //     red.posalji({3, 30});
    // } catch (const std::logic_error& e) {
    //     std::cout << "slanje posle zatvaranja: " << e.what() << '\n';
    // }

    // Korak 2 -- otkomentariši:
    // std::cout << "== korak 2: jedan proizvođač, jedan potrošač (std::thread)\n";
    // BezbedanRed<Merenje> r2;
    // std::map<int, Zbir> zbir2;
    // std::thread potr([&] { zbir2 = potrosac(r2); });
    // std::thread proiz([&] {
    //     proizvodjac(r2, 7, 1000);
    //     r2.zatvori();
    // });
    // proiz.join();
    // potr.join();
    // std::cout << "senzor 7: " << zbir2[7].n << " merenja, suma " << zbir2[7].suma << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << "== korak 3: tri proizvođača, dva potrošača (std::async)\n";
    // ispisi(pokreni(2, {{1, 500}, {2, 300}, {3, 200}}, 0, -1));

    // Korak 4 -- otkomentariši:
    // std::cout << "== korak 4: senzor 2 otkaže posle 50 merenja\n";
    // ispisi(pokreni(3, {{1, 500}, {2, 300}, {3, 200}}, 2, 50));
}

/* OČEKIVANI IZLAZ
== korak 1: red u jednoj niti
primljeno 10, 20, posle zatvaranja prazan: true
slanje posle zatvaranja: slanje u zatvoren red
== korak 2: jedan proizvođač, jedan potrošač (std::thread)
senzor 7: 1000 merenja, suma 1199500
== korak 3: tri proizvođača, dva potrošača (std::async)
  senzor 1: 500 merenja, suma 174750
  senzor 2: 300 merenja, suma 104850
  senzor 3: 200 merenja, suma 79900
  ukupno 1000 merenja
== korak 4: senzor 2 otkaže posle 50 merenja
  senzor 1: 500 merenja, suma 174750
  senzor 2: 50 merenja, suma 11225
  senzor 3: 200 merenja, suma 79900
  ukupno 750 merenja; greška: senzor 2: nema odgovora
*/
