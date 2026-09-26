// KIND: usage
//
// Zadatak 1 -- tri vrste callback-a u std::function, i lambde sa STL
// algoritmima (sekcije 1, 2, 3, 5, 9)
//   ./build.sh 5-functions-as-values/30-lambdas/exercises/ex1_slusaoci.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_slusaoci.cpp
//
// Korak 1: class Dogadjaj -- lista slušalaca
//   std::vector<std::function<void(int)>>, metoda
//   void pretplati(std::function<void(int)> f) i void objavi(int v) koja
//   pozove sve slušaoce redom.
// Korak 2: pretplati tri vrste callback-a:
//   a) običnu funkciju void loguj(int v) -- ispiše "log: v";
//   b) funkcijski objekat struct Alarm { int prag; void operator()(int v) const; }
//      -- ispiše "ALARM: v > prag" kad je v veće od praga;
//   c) dve lambde: jedna broji objave (capture brojača PO REFERENCI), druga
//      pamti vrednosti u std::vector<int> istorija (isto po referenci).
//   Zašto ovde referenca? Šta bi se desilo sa [brojObjava]?
// Korak 3: nad istorijom, sa lambdama: std::find_if -- prva vrednost
//   iznad praga 6 (prag zarobi po vrednosti); std::count_if -- koliko je
//   parnih.

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

// TODO korak 1, 2 (loguj, Alarm, Dogadjaj)

int main() {
    // Korak 1 i 2 -- otkomentariši:
    // Dogadjaj temp;
    // int brojObjava = 0;
    // std::vector<int> istorija;
    // temp.pretplati(loguj);
    // temp.pretplati(Alarm{10});
    // temp.pretplati([&brojObjava](int) { ++brojObjava; });
    // temp.pretplati([&istorija](int v) { istorija.push_back(v); });
    // temp.objavi(5);
    // temp.objavi(12);
    // std::cout << "objava: " << brojObjava << ", istorija:";
    // for (int v : istorija) std::cout << ' ' << v;
    // std::cout << '\n';

    // Korak 3 -- otkomentariši:
    // int prag = 6;
    // auto it = std::find_if(istorija.begin(), istorija.end(), [prag](int v) { return v > prag; });
    // std::cout << "prvi iznad " << prag << ": " << (it != istorija.end() ? *it : -1) << '\n';
    // std::cout << "parnih: " << std::count_if(istorija.begin(), istorija.end(), [](int v) { return v % 2 == 0; })
    //           << '\n';
}

/* EXPECTED OUTPUT
log: 5
log: 12
ALARM: 12 > 10
objava: 2, istorija: 5 12
prvi iznad 6: 12
parnih: 1
*/
