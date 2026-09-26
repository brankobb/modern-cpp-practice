// Rešenje zadatka ex1_input_statistics.

#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

// Korak 1: više rezultata -> struct (F.20), ne gomila izlaznih parametara.
struct Statistics {
    int valid = 0;
    int errors = 0;
    long long sum = 0;   // 2000000000 + 2000000000 ne staje u int (UB!)
    int min = 0;
    int max = 0;
};

// Korak 2: proveri svako čitanje. Neuspeh: failbit -> clear() -> preskoči
// loš token. Kraj ulaza: eof -> izlaz iz petlje.
Statistics accumulate(std::istream& in) {
    Statistics s;
    int x = 0;
    while (true) {
        if (in >> x) {
            if (s.valid == 0 || x < s.min) s.min = x;
            if (s.valid == 0 || x > s.max) s.max = x;
            s.sum += x;
            ++s.valid;
        } else if (in.eof()) {
            break;
        } else {
            ++s.errors;
            in.clear();
            in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
        }
    }
    return s;
}

// Korak 3: setw važi samo za SLEDEĆI ispis, a left/right, fixed i
// setprecision ostaju -- zato ih na kraju vraćamo.
void print(const Statistics& s) {
    auto row = [](const char* label) -> std::ostream& {
        return std::cout << std::left << std::setw(12) << label << std::right << std::setw(12);
    };
    row("valid") << s.valid << '\n';
    row("invalid") << s.errors << '\n';
    row("sum") << s.sum << '\n';
    row("min") << s.min << '\n';
    row("max") << s.max << '\n';
    double average = s.valid ? static_cast<double>(s.sum) / s.valid : 0.0;
    row("average") << std::fixed << std::setprecision(2) << average << '\n';
    std::cout.unsetf(std::ios::floatfield | std::ios::adjustfield);
    std::cout << std::setprecision(6);
}

int main() {
    std::istringstream input("12 7 x -3 99999999999 2000000000 2000000000 abc 5");
    Statistics s = accumulate(input);
    print(s);
    std::cout << 42 << " (format restored after printing)\n";
}
