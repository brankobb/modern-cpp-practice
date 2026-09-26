// Rešenje završne vežbe dela 1: izveštaj o merenjima.

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

// Jedna tabela za sve nazive; static_assert čuva da prati enum.
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
// Ceo tekst mora da bude broj: "21.5" da, "2x" i "" ne.
bool readNumber(const std::string& text, double& out) {
    std::istringstream in(text);
    char extra = 0;
    return (in >> out) && !(in >> extra);
}

// "channel temp min=-20 max=60 unit=C"
bool parseChannel(const std::string& line, Channel& c) {
    std::istringstream in(line);
    std::string word;
    if (!(in >> word) || word != "channel" || !(in >> c.name)) return false;
    bool hasMin = false, hasMax = false;
    while (in >> word) {
        const auto eq = word.find('=');
        if (eq == std::string::npos) return false;
        const std::string key = word.substr(0, eq);
        const std::string value = word.substr(eq + 1);
        if (key == "min")
            hasMin = readNumber(value, c.min);
        else if (key == "max")
            hasMax = readNumber(value, c.max);
        else if (key == "unit")
            c.unit = value;
        else
            return false;
    }
    return hasMin && hasMax && c.min < c.max;
}

// ---------------------------------------------------------------- korak 2
int findChannel(const std::vector<Channel>& channels, const std::string& name) {
    for (std::size_t i = 0; i < channels.size(); ++i)
        if (channels[i].name == name) return static_cast<int>(i);
    return -1;
}

void recordError(Data& d, Error e) { ++d.errors[static_cast<std::size_t>(e)]; }

void processMeasurement(Data& d, const std::string& line) {
    ++d.measurementLines;
    std::istringstream in(line);
    std::string name, text, extra;
    if (!(in >> name >> text) || (in >> extra)) return recordError(d, Error::BadFormat);
    const int c = findChannel(d.channels, name);
    if (c < 0) return recordError(d, Error::UnknownChannel);
    double v = 0;
    if (!readNumber(text, v)) return recordError(d, Error::NotANumber);
    const Channel& channel = d.channels[static_cast<std::size_t>(c)];
    if (v < channel.min || v > channel.max) return recordError(d, Error::OutOfRange);
    d.measurements.push_back({static_cast<std::size_t>(c), v});
}

Data load(std::istream& input) {
    Data d;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (line.compare(0, 8, "channel ") == 0) {
            Channel c;
            if (parseChannel(line, c))
                d.channels.push_back(c);
            else
                ++d.badConfigLines;
        } else {
            processMeasurement(d, line);
        }
    }
    return d;
}

// ---------------------------------------------------------------- korak 3
struct Stats {
    int n = 0;
    double min = 0, max = 0, sum = 0;
};

std::vector<Stats> stats(const Data& d) {
    std::vector<Stats> s(d.channels.size());
    for (const auto& [channel, v] : d.measurements) {
        Stats& st = s[channel];
        if (st.n == 0 || v < st.min) st.min = v;
        if (st.n == 0 || v > st.max) st.max = v;
        st.sum += v;
        ++st.n;
    }
    return s;
}

// ---------------------------------------------------------------- korak 4
// Overload: isti posao ("kolona širine w"), različit tip.
void column(std::ostream& out, double v, int w) { out << std::setw(w) << std::fixed << std::setprecision(1) << v; }
void column(std::ostream& out, const std::string& s, int w) { out << std::setw(w) << s; }

void printReport(std::ostream& out, const Data& d) {
    const int valid = static_cast<int>(d.measurements.size());
    out << "channels: " << d.channels.size() << " (invalid configuration lines: " << d.badConfigLines << ")\n";
    out << "measurements: " << valid << " valid of " << d.measurementLines << '\n';
    out << "errors:";
    const char* sep = " ";
    for (std::size_t i = 0; i < d.errors.size(); ++i) {
        out << sep << errorName(static_cast<Error>(i)) << ' ' << d.errors[i];
        sep = ", ";
    }
    out << "\n\n" << std::left << std::setw(10) << "channel" << std::right << std::setw(3) << "n" << std::setw(10)
        << "min" << std::setw(10) << "max" << std::setw(10) << "average" << '\n';
    const auto st = stats(d);
    for (std::size_t i = 0; i < d.channels.size(); ++i) {
        const Channel& c = d.channels[i];
        out << std::left << std::setw(10) << c.name << std::right << std::setw(3) << st[i].n;
        if (st[i].n > 0) {
            column(out, st[i].min, 10);
            column(out, st[i].max, 10);
            column(out, st[i].sum / st[i].n, 10);
        } else {
            for (int j = 0; j < 3; ++j) column(out, "-", 10);
        }
        out << ' ' << c.unit << '\n';
    }
}

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
