// VRSTA: upotreba
//
// Zadatak 1 -- inicijalizacija članova (sekcije 8, 9, 14)
//   ./build.sh 1-osnove-jezika/03-inicijalizacija/exercises/z1_uart_config.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_uart_config.cpp
//
// Korak 1: UartConfig je AGREGAT (bez konstruktora) sa podrazumevanim
//   vrednostima članova: baud = 9600, parity = 'N', stopBits = 1.
//   Tada UartConfig{}, UartConfig{115200} i UartConfig{115200, 'E', 2}
//   rade bez ijednog konstruktora.
// Korak 2: klasa Uart ima tri člana:
//   const int id_;        -- ne menja se posle pravljenja
//   UartConfig cfg_;
//   int& txCounter_;      -- deli brojač sa pozivaocem
//   Konstruktor Uart(int id, UartConfig cfg, int& txCounter). Pitanje:
//   zašto const i referencu MORAŠ da postaviš u init listi, a ne u telu?
//   (probaj u telu i pročitaj grešku)
// Korak 3: dodaj član bool open_ = false; (NSDMI), metode open() i
//   send(const char*) -- send broji poslate poruke u txCounter_ samo ako je
//   port otvoren, i print() koji ispisuje "uart <id>: <baud> <parity>
//   <stopBits>, poslato <n>".

#include <iostream>

struct UartConfig {
    // TODO korak 1
};

class Uart {
public:
    // TODO korak 2 i 3
};

int main() {
    // Korak 1 -- otkomentariši:
    // UartConfig a{};
    // UartConfig b{115200};
    // UartConfig c{115200, 'E', 2};
    // for (const UartConfig* p : {&a, &b, &c})
    //     std::cout << p->baud << ' ' << p->parity << ' ' << p->stopBits << '\n';

    // Korak 2 i 3 -- otkomentariši:
    // int sent = 0;
    // Uart u(3, c, sent);
    // u.send("pre open");   // port zatvoren: ne broji se
    // u.open();
    // u.send("a");
    // u.send("b");
    // u.print();
    // std::cout << "brojač kod pozivaoca: " << sent << '\n';
}

/* OČEKIVANI IZLAZ
9600 N 1
115200 N 1
115200 E 2
uart 3: 115200 E 2, poslato 2
brojač kod pozivaoca: 2
*/
