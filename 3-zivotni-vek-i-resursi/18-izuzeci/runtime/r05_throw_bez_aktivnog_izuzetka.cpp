// EXPECT-RUN: terminate called without an active exception
// NIJE UB, ali program se prekine: "throw;" (bez izraza) ponovo baca
// TRENUTNI izuzetak. Van catch bloka (ili funkcije pozvane iz njega)
// trenutnog izuzetka nema, pa se poziva std::terminate.
// Ispravno: "throw;" samo unutar catch-a (main.cpp, sekcija 5).
#include <iostream>
void prosledi() { throw; }
int main() {
    std::cout << "start\n";
    prosledi();
}
