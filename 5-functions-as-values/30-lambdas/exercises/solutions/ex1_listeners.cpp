// Rešenje zadatka ex1_listeners.

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

// Korak 2a: obična funkcija -- bez stanja.
void logValue(int v) { std::cout << "log: " << v << '\n'; }

// Korak 2b: funkcijski objekat -- stanje (prag) u članu.
struct Alarm {
    int threshold;
    void operator()(int v) const {
        if (v > threshold) std::cout << "ALARM: " << v << " > " << threshold << '\n';
    }
};

// Korak 1: std::function<void(int)> prima SVE što se može pozvati sa int:
// funkciju, objekat sa operator(), lambdu -- zato slušaoci mogu biti
// različitih vrsta u istom vektoru.
class Event {
public:
    void subscribe(std::function<void(int)> f) { listeners_.push_back(std::move(f)); }
    void publish(int v) const {
        for (const auto& f : listeners_) f(v);
    }

private:
    std::vector<std::function<void(int)>> listeners_;
};

int main() {
    Event temp;
    int publishCount = 0;
    std::vector<int> history;
    temp.subscribe(logValue);
    temp.subscribe(Alarm{10});
    // Korak 2c: po REFERENCI, jer lambda treba da menja promenljive
    // pozivaoca. Sa [publishCount] bi menjala svoju kopiju (i tražila
    // mutable), a publishCount u main-u bi ostao 0. Referenca je ovde
    // bezbedna, jer se publish() poziva samo dok su publishCount i history
    // žive. (Da temp nadživi te promenljive, reference bi visile -- ex3.)
    temp.subscribe([&publishCount](int) { ++publishCount; });
    temp.subscribe([&history](int v) { history.push_back(v); });
    temp.publish(5);
    temp.publish(12);
    std::cout << "published: " << publishCount << ", history:";
    for (int v : history) std::cout << ' ' << v;
    std::cout << '\n';

    // Korak 3: threshold po vrednosti -- lambda ga samo čita.
    int threshold = 6;
    auto it = std::find_if(history.begin(), history.end(), [threshold](int v) { return v > threshold; });
    std::cout << "first above " << threshold << ": " << (it != history.end() ? *it : -1) << '\n';
    std::cout << "even: " << std::count_if(history.begin(), history.end(), [](int v) { return v % 2 == 0; })
              << '\n';
}
