// KIND: why
// DEMO-UB: NAIVE stack-use-after-scope
//
// Zadatak 3 -- zašto [&] za callback koji se čuva za kasnije visi
// (sekcije 5, 6; lekcija 27, sekcija 7)
// Rešenje: exercises/solutions/ex3_reference_in_loop.cpp
//
// U petlji se pravi lista zadataka (callback-ova) koji se izvrše kasnije.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 5-functions-as-values/30-lambdas/exercises/ex3_reference_in_loop.cpp -DNAIVE
//   ASan prijavi stack-use-after-scope. Lambda sa [&] zarobi REFERENCU na
//   id -- promenljivu tela petlje, koja nestaje na kraju svake iteracije.
//   Zadaci se izvršavaju posle petlje, kad nijedan id više ne postoji.
//   [&] je bezbedan samo kad se lambda izvrši dok su promenljive žive
//   (npr. odmah, u std::sort ili std::count_if).
// Korak 2: u #else grani napravi zadatke tako da nose svoju kopiju: [id]
//   ili [=]. Za string koji se samo premešta u lambdu:
//   [name = std::move(name)] (sekcija 8).

#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

int main() {
    std::vector<std::function<void()>> tasks;
    for (int i = 0; i < 3; ++i) {
        int id = 100 + i;
#ifdef NAIVE
        tasks.push_back([&] { std::cout << "task " << id << '\n'; });
#else
        // TODO korak 2
        (void)id;
#endif
    }
    for (const auto& z : tasks) z();
}

/* EXPECTED OUTPUT
task 100
task 101
task 102
*/
