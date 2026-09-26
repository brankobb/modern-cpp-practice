// FLAGS: -Werror
// EXPECT-GCC: exception of type 'SensorError' will be caught by earlier handler
// EXPECT-CLANG: exception of type 'const SensorError &' will be caught by earlier handler
// POGREŠNO: catch blokovi se probaju REDOM, i pobeđuje PRVI koji odgovara
// (ne "najbolji", kao kod overload-a). Bazna klasa ispred izvedene znači
// da izvedeni catch nikad ne može da se izvrši. Oba kompajlera upozore
// (-Wexceptions); ovde je -Werror, pa je greška.
// Ispravno: od najspecifičnijeg ka najopštijem (main.cpp, sekcija 2).
#include <iostream>
#include <stdexcept>
struct SensorError : std::runtime_error {
    using std::runtime_error::runtime_error;
};
int main() {
    try {
        throw SensorError("timeout");
    } catch (const std::runtime_error& e) {
        std::cout << "general: " << e.what() << '\n';
    } catch (const SensorError& e) {
        std::cout << "sensor: " << e.what() << '\n';
    }
}
