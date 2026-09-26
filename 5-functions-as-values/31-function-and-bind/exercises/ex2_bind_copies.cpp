// KIND: why
// DEMO-OUT: NAIVE messages sent: 0
//
// Zadatak 2 -- zašto bind "ne menja" promenljivu (sekcija 4)
// Rešenje: exercises/solutions/ex2_bind_copies.cpp
//
// send(int& counter, const char* message) šalje poruku i uveća brojač
// poslatih. Callback za dugme je napravljen bind-om.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 5-functions-as-values/31-function-and-bind/exercises/ex2_bind_copies.cpp -DNAIVE
//   Dugme je kliknuto tri puta, a brojač u main-u je 0. bind KOPIRA sve
//   argumente u sebe (i sent), i send() dobija referencu na tu
//   unutrašnju kopiju -- koju onda uvećava. Nema greške ni upozorenja:
//   int& se lepo veže za kopiju.
// Korak 2: u #else grani napravi callback koji menja PRAVI brojač, na dva
//   načina: a) std::bind sa std::ref(sent); b) lambda [&sent].
//   Lambda jasno kaže šta se hvata po referenci, a bind to sakrije.

#include <functional>
#include <iostream>

void send(int& counter, const char* message) {
    std::cout << "sending: " << message << '\n';
    ++counter;
}

int main() {
    int sent = 0;
    std::function<void()> onClick;
#ifdef NAIVE
    onClick = std::bind(send, sent, "ping");
#else
    // TODO korak 2
    onClick = [] {};
#endif
    onClick();
    onClick();
    onClick();
    std::cout << "messages sent: " << sent << '\n';
}

/* EXPECTED OUTPUT
sending: ping
sending: ping
sending: ping
messages sent: 3
*/
