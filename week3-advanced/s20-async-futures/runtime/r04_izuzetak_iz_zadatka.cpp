// EXPECT-RUN: terminate called after throwing an instance of 'std::runtime_error'
// POGREŠNO: get() bez try. Sam izuzetak iz zadatka NIJE izgubljen (za
// razliku od std::thread, s19 runtime/r02): async ga sačuva u future, a
// get() ga baci ponovo -- u niti koja zove get, ovde main. Program se
// prekida samo zato što ga main ne hvata.
// Ispravno: try { f.get(); } catch (const std::exception& e) { ... }
// (main.cpp, sekcija 6).
#include <future>
#include <stdexcept>
int main() {
    auto f = std::async(std::launch::async, []() -> int { throw std::runtime_error("kalibracija nije uspela"); });
    return f.get();
}
