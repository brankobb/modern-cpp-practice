// EXPECT-UB: heap-use-after-free
// POGREŠNO: destruktor globalnog objekta koristi static lokalni objekat koji
//   je napravljen KASNIJE.
// Zašto: objekti sa static trajanjem se uništavaju obrnutim redom od
//   završetka konstrukcije ([basic.start.term]). logger je napravljen pre
//   main, a name u registryName() tek pri prvom pozivu, u main. Zato se name
//   uništi PRE loggera, i ~Logger čita oslobođen string. Isti problem kao
//   redosled inicijalizacije (lekcija 08, ub/u01), samo na izlazu.
// Ispravno: ne koristi druge static objekte iz destruktora static objekta;
//   ili pozovi registryName() iz KONSTRUKTORA Logger-a, pa se name napravi
//   pre loggera i uništi posle njega; ili (poslednja opcija) static objekat
//   koji se namerno nikad ne uništava: static auto* name = new std::string(...).
#include <cstdio>
#include <string>

std::string& registryName() {
    static std::string name(64, 'r');
    return name;
}

struct Logger {
    ~Logger() { std::printf("Logger gasi: %c\n", registryName().c_str()[0]); }
};

Logger logger;

int main() {
    registryName();
    std::printf("main\n");
}
