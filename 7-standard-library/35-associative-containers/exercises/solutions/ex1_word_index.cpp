// Rešenje zadatka ex1_word_index.

#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Position {
    int x, y;
    bool operator==(const Position& o) const { return x == o.x && y == o.y; }
};

// Korak 3: heš uz tip; jednaki objekti MORAJU imati jednak heš.
struct PositionHash {
    std::size_t operator()(const Position& p) const noexcept {
        std::size_t h = std::hash<int>{}(p.x);
        return h ^ (std::hash<int>{}(p.y) + 0x9e3779b9 + (h << 6) + (h >> 2));
    }
};

// Korak 1: map je sortirana po ključu -- ispis je abecedni.
std::map<std::string, int> wordCount(const std::vector<std::string>& lines) {
    std::map<std::string, int> m;
    for (const auto& line : lines) {
        std::istringstream in(line);
        std::string word;
        while (in >> word) ++m[word];
    }
    return m;
}

// Korak 2: set<int> kao vrednost -- sortirano, bez duplikata.
std::map<std::string, std::set<int>> wordIndex(const std::vector<std::string>& lines) {
    std::map<std::string, std::set<int>> m;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        std::istringstream in(lines[i]);
        std::string word;
        while (in >> word) m[word].insert(static_cast<int>(i) + 1);
    }
    return m;
}

int main() {
    std::vector<std::string> lines{"temp rises", "pressure falls temp rises", "temp temp stable"};
    const char* sep = "";
    for (const auto& [word, n] : wordCount(lines)) {
        std::cout << sep << word << '=' << n;
        sep = " ";
    }
    std::cout << '\n';

    for (const auto& [word, where] : wordIndex(lines)) {
        std::cout << word << ':';
        for (int r : where) std::cout << ' ' << r;
        std::cout << '\n';
    }

    std::unordered_map<Position, std::string, PositionHash> grid{{{0, 0}, "temp"}, {{2, 1}, "humidity"}};
    grid[{1, 1}] = "pressure";
    std::cout << "(2, 1): " << grid.at({2, 1}) << ", (1, 1): " << grid.at({1, 1})
              << ", occupied (5, 5): " << grid.count({5, 5}) << '\n';
}
