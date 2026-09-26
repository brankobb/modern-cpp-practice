// KIND: why
// DEMO-OUT: NAIVE sending command 2 \(Reset\)
// DEMO-ERR: ENUM_CLASS cannot convert|no matching function
//
// Zadatak 3 -- zašto enum class (sekcija 4, EMC Item 10)
// Rešenje: exercises/solutions/ex3_enum_class.cpp
//
// Uređaj prijavljuje stanje (State), a kontroleru šalje komande (Command).
// Stari C API prima komandu kao int.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/05-compound-types/exercises/ex3_enum_class.cpp -DNAIVE
//   Programer je hteo da prijavi stanje Fault, a greškom je pozvao
//   send(). Kompajler ćuti, a uređaj dobije komandu Reset (obe
//   vrednosti su 2). Obični enum se tiho konvertuje u int.
// Korak 2: ista greška sa enum class:
//     ./build.sh .../ex3_enum_class.cpp -DENUM_CLASS
//   Sada je greška pri kompajliranju: enum class se NE konvertuje sam, pa
//   State ne može da uđe tamo gde se očekuje Command.
// Korak 3: u #else grani napiši enum class State i enum class Command
//   (oba sa podloženim tipom std::uint8_t), const char* name(Command) sa
//   switch-om, i void send(Command c) koja ispiše i bajt koji ide na
//   žicu -- tu broj tražiš EKSPLICITNO (static_cast). Otkomentariši test.

#include <cstdint>
#include <iostream>

#if defined(NAIVE)
enum State { Off, Running, Fault };           // 0 1 2
enum Command { Stop, Start, Reset };          // 0 1 2
const char* const commandNames[] = {"Stop", "Start", "Reset"};

void send(int command) {
    std::cout << "sending command " << command << " (" << commandNames[command] << ")\n";
}

int main() {
    State s = Fault;
    send(s);           // hteo sam reportState(s)...
}
#elif defined(ENUM_CLASS)
enum class State { Off, Running, Fault };
enum class Command { Stop, Start, Reset };

void send(Command) {}

int main() {
    State s = State::Fault;
    send(s);           // greška: State nije Command
}
#else
// TODO korak 3

int main() {
    // Korak 3 -- otkomentariši:
    // send(Command::Reset);
    // send(Command::Start);
    // std::cout << "sizeof(Command) = " << sizeof(Command) << '\n';
}
#endif

/* EXPECTED OUTPUT
sending command Reset, byte 2
sending command Start, byte 1
sizeof(Command) = 1
*/
