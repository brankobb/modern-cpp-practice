// Rešenje završne vežbe dela 7: indeks log fajlova.

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace fs = std::filesystem;

template <typename... F>
struct Overloaded : F... {
    using F::operator()...;
};
template <typename... F>
Overloaded(F...) -> Overloaded<F...>;

// ---------------------------------------------------------------- korak 1
// Svi .log fajlovi ispod korena, rekurzivno, sortirani (redosled obilaska
// nije određen).
std::vector<fs::path> findLogs(const fs::path& root) {
    std::vector<fs::path> logs;
    for (const auto& e : fs::recursive_directory_iterator(root))
        if (e.is_regular_file() && e.path().extension() == ".log") logs.push_back(e.path());
    std::sort(logs.begin(), logs.end());
    return logs;
}

// ---------------------------------------------------------------- korak 2
struct Record {
    std::string time, channel;
    double value;
};
struct Error {
    std::string reason;
};
using Line = std::variant<Record, Error>;

// Sledeća reč iz pogleda (razmaci se preskaču); pogled se skraćuje.
std::string_view nextWord(std::string_view& s) {
    const auto start = s.find_first_not_of(' ');
    if (start == std::string_view::npos) {
        s = {};
        return {};
    }
    s.remove_prefix(start);
    const auto stop = std::min(s.find(' '), s.size());
    const std::string_view word = s.substr(0, stop);
    s.remove_prefix(stop);
    return word;
}

std::optional<double> toNumber(std::string_view s) {
    double v = 0;
    const auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
    if (ec != std::errc() || ptr != s.data() + s.size()) return std::nullopt;
    return v;
}

// "08:15 temp 22.0"
Line parse(std::string_view line) {
    const auto time = nextWord(line);
    const auto channel = nextWord(line);
    const auto number = nextWord(line);
    if (number.empty() || !nextWord(line).empty()) return Error{"bad format"};
    const auto v = toNumber(number);
    if (!v) return Error{"not a number"};
    return Record{std::string(time), std::string(channel), *v};
}

// ---------------------------------------------------------------- korak 3
struct Index {
    std::map<std::string, std::vector<double>> byChannel;
    std::map<std::string, std::set<std::string>> channelsByFile;   // ključ: putanja relativno od korena
    std::unordered_map<std::string, int> errors;                   // razlog -> koliko puta
    int records = 0;
};

Index buildIndex(const fs::path& root) {
    Index ix;
    for (const fs::path& p : findLogs(root)) {
        const std::string name = p.lexically_relative(root).generic_string();
        std::set<std::string>& channels = ix.channelsByFile[name];
        std::ifstream in(p);
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::visit(Overloaded{
                           [&](const Record& z) {
                               ix.byChannel[z.channel].push_back(z.value);
                               channels.insert(z.channel);
                               ++ix.records;
                           },
                           [&](const Error& g) { ++ix.errors[g.reason]; },
                       },
                       parse(line));
        }
    }
    return ix;
}

// ---------------------------------------------------------------- korak 4
std::optional<double> threshold(const std::string& channel) {
    static const std::map<std::string, double> thresholds{{"temp", 80.0}, {"pressure", 2.0}};
    const auto it = thresholds.find(channel);
    if (it == thresholds.end()) return std::nullopt;
    return it->second;
}

double median(std::vector<double> v) {   // kopija: nth_element menja redosled
    const auto middle = v.begin() + static_cast<std::ptrdiff_t>(v.size() / 2);
    std::nth_element(v.begin(), middle, v.end());
    return *middle;
}

void report(const Index& ix) {
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "records: " << ix.records << '\n';
    for (const auto& [channel, v] : ix.byChannel) {
        const auto [mn, mx] = std::minmax_element(v.begin(), v.end());
        std::cout << "  " << std::left << std::setw(9) << channel << std::right << " n=" << v.size() << " min=" << *mn
                  << " max=" << *mx << " median=" << median(v);
        if (const auto p = threshold(channel))
            std::cout << " above " << *p << ": "
                      << std::count_if(v.begin(), v.end(), [p](double x) { return x > *p; });
        std::cout << '\n';
    }

    // unordered_map nema redosled: za ispis sortiraj.
    std::vector<std::pair<std::string, int>> errors(ix.errors.begin(), ix.errors.end());
    std::sort(errors.begin(), errors.end());
    std::cout << "errors:";
    for (const auto& [reason, n] : errors) std::cout << ' ' << reason << '=' << n;
    std::cout << '\n';

    std::vector<double> temp = ix.byChannel.at("temp");
    const auto k = std::min<std::size_t>(3, temp.size());
    std::partial_sort(temp.begin(), temp.begin() + static_cast<std::ptrdiff_t>(k), temp.end(), std::greater<>{});
    std::cout << "top 3 temp:";
    for (std::size_t i = 0; i < k; ++i) std::cout << ' ' << temp[i];
    std::cout << '\n';

    // Kanali koji postoje u SVIM fajlovima: presek skupova, jedan po jedan.
    auto it = ix.channelsByFile.begin();
    std::set<std::string> common = it->second;
    for (++it; it != ix.channelsByFile.end(); ++it) {
        std::set<std::string> intersection;
        std::set_intersection(common.begin(), common.end(), it->second.begin(), it->second.end(),
                              std::inserter(intersection, intersection.end()));
        common = std::move(intersection);
    }
    std::cout << "in all files:";
    for (const auto& channel : common) std::cout << ' ' << channel;
    std::cout << '\n';
}

// ---------------------------------------------------------------- podaci
fs::path makeLogs() {
    const auto number = std::chrono::steady_clock::now().time_since_epoch().count();
    const fs::path root = fs::temp_directory_path() / ("mcpp_zv7_" + std::to_string(number));
    fs::create_directories(root / "archive");
    std::ofstream(root / "hall.log") << "08:00 temp 21.5\n08:00 humidity 40\n08:15 temp 22.0\n# read manually\n"
                                         "08:30 temp abc\n08:30 humidity 42\n";
    std::ofstream(root / "boiler.log") << "08:00 temp 78.0\n08:05 pressure 1.8\n08:10 temp 83.5\n08:15 temp 81.0\n"
                                          "08:20 pressure 2.4\n08:25\n";
    std::ofstream(root / "archive" / "2024-01.log") << "07:00 temp 19.0\n07:30 humidity 55 %\n07:45 temp 18.5\n";
    std::ofstream(root / "note.txt") << "not a log\n";
    return root;
}

int main() {
    const fs::path root = makeLogs();

    std::cout << "== step 1: finding logs\n";
    for (const auto& p : findLogs(root)) std::cout << "  " << p.lexically_relative(root).generic_string() << '\n';

    std::cout << "== step 2: parsing a line\n";
    for (const char* line : {"08:15 temp 22.0", "08:30 temp abc", "08:25", "07:30 humidity 55 %"}) {
        std::cout << "  \"" << line << "\" -> ";
        std::visit(Overloaded{
                       [](const Record& z) { std::cout << z.channel << " = " << z.value << " at " << z.time << '\n'; },
                       [](const Error& g) { std::cout << "error: " << g.reason << '\n'; },
                   },
                   parse(line));
    }

    std::cout << "== step 3: index\n";
    const Index ix = buildIndex(root);
    for (const auto& [file, channels] : ix.channelsByFile) {
        std::cout << "  " << file << ':';
        for (const auto& channel : channels) std::cout << ' ' << channel;
        std::cout << '\n';
    }

    std::cout << "== step 4: report\n";
    report(ix);

    fs::remove_all(root);   // uvek: briše privremeni direktorijum
}
