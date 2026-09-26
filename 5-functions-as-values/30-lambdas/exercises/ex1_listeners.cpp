// KIND: usage
//
// Zadatak 1 -- tri vrste callback-a u std::function, i lambde sa STL
// algoritmima (sekcije 1, 2, 3, 5, 9)
//   ./build.sh 5-functions-as-values/30-lambdas/exercises/ex1_listeners.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_listeners.cpp
//
// Korak 1: class Event -- lista slušalaca
//   std::vector<std::function<void(int)>>, metoda
//   void subscribe(std::function<void(int)> f) i void publish(int v) koja
//   pozove sve slušaoce redom.
// Korak 2: subscribe tri vrste callback-a:
//   a) običnu funkciju void logValue(int v) -- ispiše "log: v";
//   b) funkcijski objekat struct Alarm { int threshold; void operator()(int v) const; }
//      -- ispiše "ALARM: v > threshold" kad je v veće od praga;
//   c) dve lambde: jedna broji objave (capture brojača PO REFERENCI), druga
//      pamti vrednosti u std::vector<int> history (isto po referenci).
//   Zašto ovde referenca? Šta bi se desilo sa [publishCount]?
// Korak 3: nad istorijom, sa lambdama: std::find_if -- prva vrednost
//   iznad praga 6 (prag zarobi po vrednosti); std::count_if -- koliko je
//   parnih.

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

// TODO korak 1, 2 (logValue, Alarm, Event)

int main() {
    // Korak 1 i 2 -- otkomentariši:
    // Event temp;
    // int publishCount = 0;
    // std::vector<int> history;
    // temp.subscribe(logValue);
    // temp.subscribe(Alarm{10});
    // temp.subscribe([&publishCount](int) { ++publishCount; });
    // temp.subscribe([&history](int v) { history.push_back(v); });
    // temp.publish(5);
    // temp.publish(12);
    // std::cout << "published: " << publishCount << ", history:";
    // for (int v : history) std::cout << ' ' << v;
    // std::cout << '\n';

    // Korak 3 -- otkomentariši:
    // int threshold = 6;
    // auto it = std::find_if(history.begin(), history.end(), [threshold](int v) { return v > threshold; });
    // std::cout << "first above " << threshold << ": " << (it != history.end() ? *it : -1) << '\n';
    // std::cout << "even: " << std::count_if(history.begin(), history.end(), [](int v) { return v % 2 == 0; })
    //           << '\n';
}

/* EXPECTED OUTPUT
log: 5
log: 12
ALARM: 12 > 10
published: 2, history: 5 12
first above 6: 12
even: 1
*/
