// Završna vežba dela 2 -- temperature i senzori
//   ./build.sh 2-classes/capstone/task.cpp
// Uputstvo i spisak lekcija: 2-classes/capstone/notes.md
// Rešenje: solution.cpp (otvori tek kad tvoj izlaz bude isti kao blok
// EXPECTED OUTPUT na dnu fajla).
//
// Fajl se kompajlira i ovakav. Piši klase redom; posle svakog koraka
// otkomentariši njegov deo main()-a.
//
// Korak 1: class Razlika i class Temperatura
//   -- Razlika: razlika dve temperature u K; explicit konstruktor iz
//   double (lekcija 17, sekcija 5), kelvina(), +, ==, != i <<
//   ("+2.0 K", sa predznakom).
//   -- Temperatura: čuva °C; invarijanta "nikad ispod -273.15 °C" --
//   konstruktor je PRIVATAN, a prave je fabričke funkcije izCelzijusa i
//   izKelvina (lekcija 14, sekcije 1 i 2); assert čuva invarijantu i u +=.
//   -- Temperatura + Razlika daje Temperaturu, Temperatura - Temperatura
//   daje Razliku, a Temperatura + Temperatura ne sme da se kompajlira
//   (lekcija 15, sekcije 2 i 3: slobodne friend funkcije).
//   -- ==, !=, <, > i << ("23.5 °C", jedna decimala). operator<< ne sme
//   trajno da promeni podešavanja stream-a: formatiraj u lokalni
//   std::ostringstream (lekcija 15, sekcija 6).
// Korak 2: hijerarhija senzora
//   -- apstraktna class Senzor: ime, čisto virtuelna citaj(), virtuelna
//   opis(), virtual destruktor, brojač živih kao static član, zabranjeno
//   kopiranje (lekcija 16, sekcije 4, 5 i 8; lekcija 14, sekcija 6).
//   -- SimuliraniSenzor: vraća zadate vrednosti ukrug.
//   -- KalibrisaniSenzor final: SimuliraniSenzor + pomak (Razlika);
//   citaj() poziva baznu verziju i dodaje pomak; kalibrisi(Razlika).
// Korak 3: int kalibrisiSve(const std::vector<Senzor*>&, Razlika)
//   -- kalibriše samo senzore koji to umeju: dynamic_cast (lekcija 17,
//   sekcija 4); vraća koliko ih je kalibrisano.
// Korak 4: class IznadPraga (funkcijski objekat, lekcija 15, sekcija 9)
//   i nadzor(): za svaki senzor ciklusa čitanja, najviša vrednost, porast
//   od prve i broj alarma.

#include <cassert>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// TODO korak 1, 2, 3, 4

int main() {
    // Korak 1 -- otkomentariši:
    // std::cout << "== korak 1: temperatura i razlika\n";
    // const Temperatura t = Temperatura::izCelzijusa(21.5);
    // const Temperatura t2 = t + Razlika{2.0};
    // std::cout << t << " + 2 K = " << t2 << "; razlika " << (t2 - t) << "; kelvin " << t2.kelvin() << '\n';
    // std::cout << std::boolalpha << "t < t2: " << (t < t2) << ", t == 21.5 °C: " << (t == Temperatura::izCelzijusa(21.5))
    //           << ", 273.15 K = " << Temperatura::izKelvina(273.15) << '\n';

    // Korak 2 -- otkomentariši:
    // std::cout << "== korak 2: hijerarhija senzora\n";
    // SimuliraniSenzor hala("hala", {21.5, 22.0, 22.5});
    // KalibrisaniSenzor kotao("kotao", {78.0, 81.0, 84.0}, Razlika{-1.5});
    // SimuliraniSenzor napolje("napolje", {-3.0, -1.0});
    // const std::vector<Senzor*> svi{&hala, &kotao, &napolje};   // ne poseduje: objekti su na steku
    // for (const Senzor* s : svi) std::cout << s->opis() << '\n';
    // std::cout << "živih senzora: " << Senzor::zivih() << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << "== korak 3: kalibracija preko dynamic_cast\n";
    // std::cout << "kalibrisano: " << kalibrisiSve(svi, Razlika{0.5}) << ", pomak kotla sada " << kotao.pomak() << '\n';

    // Korak 4 -- otkomentariši:
    // std::cout << "== korak 4: nadzor, prag 80 °C\n";
    // nadzor(svi, 3, IznadPraga{Temperatura::izCelzijusa(80.0)});
}

/* EXPECTED OUTPUT
== korak 1: temperatura i razlika
21.5 °C + 2 K = 23.5 °C; razlika +2.0 K; kelvin 296.65
t < t2: true, t == 21.5 °C: true, 273.15 K = 0.0 °C
== korak 2: hijerarhija senzora
senzor 'hala', simuliran, 3 vrednosti
senzor 'kotao', simuliran, 3 vrednosti, kalibrisan
senzor 'napolje', simuliran, 2 vrednosti
živih senzora: 3
== korak 3: kalibracija preko dynamic_cast
kalibrisano: 1, pomak kotla sada -1.0 K
== korak 4: nadzor, prag 80 °C
hala     najviša 22.5 °C, porast od prve +1.0 K, alarma 0
kotao    najviša 83.0 °C, porast od prve +6.0 K, alarma 1
napolje  najviša -1.0 °C, porast od prve +2.0 K, alarma 0
*/
