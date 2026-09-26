// EXPECT-UB: stack-use-after-return
// UB: future vraćen iz funkcije se premešta pozivaocu, pa se njegov
// destruktor NE izvrši na kraju pokreni() -- zadatak radi dalje. Lambda
// je zarobila referencu na lokalnu promenljivu, koja je nestala kad se
// pokreni() vratila; zadatak 50 ms kasnije čita mrtav stek.
// Ispravno: zarobi po vrednosti ([lokalna]) -- podaci koje zadatak
// koristi moraju da žive koliko i zadatak.
#include <chrono>
#include <future>
#include <iostream>
#include <thread>
std::future<int> launch() {
    int local = 5;
    return std::async(std::launch::async, [&local] {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return local * 2;
    });
}
int main() {
    auto f = launch();
    std::cout << f.get() << '\n';
}
