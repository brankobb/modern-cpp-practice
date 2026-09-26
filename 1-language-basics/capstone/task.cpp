// Završna vežba dela 1 -- izveštaj o merenjima
//   ./build.sh 1-language-basics/capstone/task.cpp
// Uputstvo i spisak lekcija: 1-language-basics/capstone/notes.md
// Rešenje: solution.cpp (otvori tek kad tvoj izlaz bude isti kao blok
// EXPECTED OUTPUT na dnu fajla).
//
// Fajl se kompajlira i ovakav: funkcije postoje, ali ne rade ništa. Piši
// ih redom; posle svakog koraka pokreni i vidi koliko izveštaja već valja.
//
// Korak 1: readNumber i parseChannel
//   -- ceo tekst mora da bude broj ("2x" nije); std::istringstream, >>, pa
//   proveri da ništa nije ostalo (lekcija 06, sekcija 5; lekcija 01,
//   sekcija 7).
//   -- red "channel temp min=-20 max=60 unit=C": reči preko >>, par
//   ključ=vrednost preko find('=') i substr (lekcija 06, sekcija 3). Kanal
//   bez min ili max, ili sa min >= max, nije ispravan.
// Korak 2: findChannel, processMeasurement, load
//   -- std::getline red po red; prazni redovi i redovi sa '#' se
//   preskaču; red koji počinje sa "channel " je konfiguracija, ostali su
//   merenja "ime vrednost". Merenje može da ima tačno jednu grešku, tim
//   redom: loš format, nepoznat kanal, nije broj, van opsega -- broji ih
//   preko recordError.
//   -- parametri: šta se samo čita ide kao const&, šta se menja kao &
//   (lekcija 04, sekcija 10; lekcija 09, sekcija 3).
// Korak 3: stats
//   -- za svaki kanal n, min, max, zbir; range-for sa structured bindings
//   preko d.measurements (lekcija 10, sekcija 7).
// Korak 4: column (dva overload-a) i printReport
//   -- std::setw, std::left/std::right, std::fixed, std::setprecision(1)
//   (lekcija 01, sekcija 8); kanal bez merenja ima "-" u kolonama;
//   overload za double i za std::string (lekcija 11, sekcija 1).

#include <array>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace measurements {

struct Channel {
    std::string name;
    double min = 0;
    double max = 0;
    std::string unit;
};

struct Measurement {
    std::size_t channel;   // indeks u vektoru kanala
    double value;
};

enum class Error { UnknownChannel, NotANumber, OutOfRange, BadFormat };

constexpr std::array<const char*, 4> kErrorNames{"unknown channel", "not a number", "out of range", "bad format"};
static_assert(kErrorNames.size() == static_cast<std::size_t>(Error::BadFormat) + 1,
              "the name table must follow enum Error");

constexpr const char* errorName(Error e) { return kErrorNames[static_cast<std::size_t>(e)]; }

struct Data {
    std::vector<Channel> channels;
    std::vector<Measurement> measurements;
    std::array<int, kErrorNames.size()> errors{};   // brojač po vrsti greške
    int measurementLines = 0;
    int badConfigLines = 0;
};

// ---------------------------------------------------------------- korak 1
// TODO: true ako je ceo tekst broj; vrednost upisuje u out.
bool readNumber(const std::string&, double&) { return false; }

// TODO: true ako je red ispravna konfiguracija kanala; popunjava c.
bool parseChannel(const std::string&, Channel&) { return false; }

// ---------------------------------------------------------------- korak 2
// TODO: indeks kanala sa tim imenom, ili -1.
int findChannel(const std::vector<Channel>&, const std::string&) { return -1; }

void recordError(Data& d, Error e) { ++d.errors[static_cast<std::size_t>(e)]; }

// TODO: jedan red merenja -- ili ispravno merenje u d.measurements, ili
// tačno jedna greška; u oba slučaja ++d.measurementLines.
void processMeasurement(Data&, const std::string&) {}

// TODO: ceo ulaz, red po red.
Data load(std::istream&) { return {}; }

// ---------------------------------------------------------------- korak 3
struct Stats {
    int n = 0;
    double min = 0, max = 0, sum = 0;
};

// TODO: jedan Stats po kanalu, istim redom kao d.channels.
std::vector<Stats> stats(const Data& d) { return std::vector<Stats>(d.channels.size()); }

// ---------------------------------------------------------------- korak 4
// TODO: dva overload-a funkcije column (double i std::string) i izveštaj.
void printReport(std::ostream&, const Data&) {}

}  // namespace measurements

const char* const kInput = R"(# configuration
channel temp min=-20 max=60 unit=C
channel humidity min=0 max=100 unit=%
channel pressure min=900 max=1100 unit=hPa
channel current min=0 max=10 unit=A
channel broken min=5

# measurements
temp 21.5
humidity 40
temp 85
pressure 1013.2
humidity abc
voltage 12
temp 22
pressure
humidity 55
temp 21.8
)";

int main() {
    std::istringstream input(kInput);
    const measurements::Data d = measurements::load(input);
    measurements::printReport(std::cout, d);
}

/* EXPECTED OUTPUT
channels: 4 (invalid configuration lines: 1)
measurements: 6 valid of 10
errors: unknown channel 1, not a number 1, out of range 1, bad format 1

channel     n       min       max   average
temp        3      21.5      22.0      21.8 C
humidity    2      40.0      55.0      47.5 %
pressure    1    1013.2    1013.2    1013.2 hPa
current     0         -         -         - A
*/
