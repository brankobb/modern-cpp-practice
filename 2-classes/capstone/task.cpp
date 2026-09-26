// Završna vežba dela 2 -- temperature i senzori
//   ./build.sh 2-classes/capstone/task.cpp
// Uputstvo i spisak lekcija: 2-classes/capstone/notes.md
// Rešenje: solution.cpp (otvori tek kad tvoj izlaz bude isti kao blok
// EXPECTED OUTPUT na dnu fajla).
//
// Fajl se kompajlira i ovakav. Piši klase redom; posle svakog koraka
// otkomentariši njegov deo main()-a.
//
// Korak 1: class Delta i class Temperature
//   -- Delta: razlika dve temperature u K; explicit konstruktor iz
//   double (lekcija 17, sekcija 5), kelvins(), +, ==, != i <<
//   ("+2.0 K", sa predznakom).
//   -- Temperature: čuva °C; invarijanta "nikad ispod -273.15 °C" --
//   konstruktor je PRIVATAN, a prave je fabričke funkcije fromCelsius i
//   fromKelvin (lekcija 14, sekcije 1 i 2); assert čuva invarijantu i u +=.
//   -- Temperature + Delta daje Temperature, Temperature - Temperature
//   daje Delta, a Temperature + Temperature ne sme da se kompajlira
//   (lekcija 15, sekcije 2 i 3: slobodne friend funkcije).
//   -- ==, !=, <, > i << ("23.5 °C", jedna decimala). operator<< ne sme
//   trajno da promeni podešavanja stream-a: formatiraj u lokalni
//   std::ostringstream (lekcija 15, sekcija 6).
// Korak 2: hijerarhija senzora
//   -- apstraktna class Sensor: ime, čisto virtuelna read(), virtuelna
//   describe(), virtual destruktor, brojač živih kao static član,
//   zabranjeno kopiranje (lekcija 16, sekcije 4, 5 i 8; lekcija 14,
//   sekcija 6).
//   -- SimulatedSensor: vraća zadate vrednosti ukrug.
//   -- CalibratedSensor final: SimulatedSensor + pomak (Delta);
//   read() poziva baznu verziju i dodaje pomak; calibrate(Delta).
// Korak 3: int calibrateAll(const std::vector<Sensor*>&, Delta)
//   -- kalibriše samo senzore koji to umeju: dynamic_cast (lekcija 17,
//   sekcija 4); vraća koliko ih je kalibrisano.
// Korak 4: class AboveThreshold (funkcijski objekat, lekcija 15, sekcija 9)
//   i monitor(): za svaki senzor ciklusa čitanja, najviša vrednost, porast
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
    // std::cout << "== step 1: temperature and delta\n";
    // const Temperature t = Temperature::fromCelsius(21.5);
    // const Temperature t2 = t + Delta{2.0};
    // std::cout << t << " + 2 K = " << t2 << "; delta " << (t2 - t) << "; kelvin " << t2.kelvin() << '\n';
    // std::cout << std::boolalpha << "t < t2: " << (t < t2) << ", t == 21.5 °C: " << (t == Temperature::fromCelsius(21.5))
    //           << ", 273.15 K = " << Temperature::fromKelvin(273.15) << '\n';

    // Korak 2 -- otkomentariši:
    // std::cout << "== step 2: sensor hierarchy\n";
    // SimulatedSensor hall("hall", {21.5, 22.0, 22.5});
    // CalibratedSensor boiler("boiler", {78.0, 81.0, 84.0}, Delta{-1.5});
    // SimulatedSensor outside("outside", {-3.0, -1.0});
    // const std::vector<Sensor*> all{&hall, &boiler, &outside};   // ne poseduje: objekti su na steku
    // for (const Sensor* s : all) std::cout << s->describe() << '\n';
    // std::cout << "sensors alive: " << Sensor::alive() << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << "== step 3: calibration via dynamic_cast\n";
    // std::cout << "calibrated: " << calibrateAll(all, Delta{0.5}) << ", boiler offset now " << boiler.offset() << '\n';

    // Korak 4 -- otkomentariši:
    // std::cout << "== step 4: monitoring, threshold 80 °C\n";
    // monitor(all, 3, AboveThreshold{Temperature::fromCelsius(80.0)});
}

/* EXPECTED OUTPUT
== step 1: temperature and delta
21.5 °C + 2 K = 23.5 °C; delta +2.0 K; kelvin 296.65
t < t2: true, t == 21.5 °C: true, 273.15 K = 0.0 °C
== step 2: sensor hierarchy
sensor 'hall', simulated, 3 values
sensor 'boiler', simulated, 3 values, calibrated
sensor 'outside', simulated, 2 values
sensors alive: 3
== step 3: calibration via dynamic_cast
calibrated: 1, boiler offset now -1.0 K
== step 4: monitoring, threshold 80 °C
hall     max 22.5 °C, rise from first +1.0 K, alarms 0
boiler   max 83.0 °C, rise from first +6.0 K, alarms 1
outside  max -1.0 °C, rise from first +2.0 K, alarms 0
*/
