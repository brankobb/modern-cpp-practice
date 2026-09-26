// Rešenje zadatka ex3_strong_guarantee.

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct Config {
    std::vector<int> channels;
    std::string name;
};

void checkName(const std::string& name) {
    if (name.empty()) throw std::invalid_argument("empty name");
}

void print(const char* label, const Config& c) {
    std::cout << label << ": channels";
    for (int ch : c.channels) std::cout << ' ' << ch;
    std::cout << ", name " << c.name << '\n';
}

// Ako menjaš original deo po deo (nije dobro): izuzetak u sredini ostavi
// objekat napola promenjen.
// Treba ovako: sve što može da baci radi na kopiji, a original se menja
// samo operacijom koja ne baca (copy-and-swap).
void applyChanges(Config& c, const std::vector<int>& channels, const std::string& name) {
    Config updated = c;           // može da baci (alokacija) -- c netaknut
    updated.channels = channels;  // može da baci -- c netaknut
    checkName(name);              // može da baci -- c netaknut
    updated.name = name;
    std::swap(c, updated);        // move vector-a i string-a: noexcept
}
// Možeš i ovako, kad je jeftinije: prvo SVE provere, pa tek onda izmene
// (ovde bi bilo dovoljno pozvati checkName pre c.channels = channels). To
// radi samo dok nijedna izmena ne može da baci -- a c.channels = channels
// može (alokacija).

int main() {
    Config c{{1, 2, 3}, "old"};
    try {
        applyChanges(c, {4, 5, 6}, "");
    } catch (const std::invalid_argument&) {
        print("after error", c);
    }
    applyChanges(c, {7, 8}, "new");
    print("after success", c);
}
