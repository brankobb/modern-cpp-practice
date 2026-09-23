// Rešenje zadatka z1_uart_config.

#include <iostream>

// Korak 1: agregat sa podrazumevanim vrednostima (C++14+ dozvoljava NSDMI u
// agregatu). Članovi koji nisu navedeni u {} uzimaju svoju podrazumevanu
// vrednost, pa UartConfig{115200} menja samo baud.
struct UartConfig {
    int baud = 9600;
    char parity = 'N';
    int stopBits = 1;
};

class Uart {
public:
    // Korak 2: const član i referenca moraju u init listu. U telu
    // konstruktora objekat je VEĆ napravljen, pa bi "id_ = id;" bila dodela
    // const objektu (greška), a referenca bez inicijalizatora ne može ni da
    // postoji ("uninitialized reference member").
    // Redosled u listi prati redosled deklaracije (C.47).
    Uart(int id, UartConfig cfg, int& txCounter)
        : id_{id}, cfg_{cfg}, txCounter_{txCounter} {}

    // Korak 3
    void open() { open_ = true; }

    void send(const char* msg) {
        if (!open_) return;
        (void)msg;          // pravi drajver bi ovde slao bajtove
        ++txCounter_;
    }

    void print() const {
        std::cout << "uart " << id_ << ": " << cfg_.baud << ' ' << cfg_.parity
                  << ' ' << cfg_.stopBits << ", poslato " << txCounter_ << '\n';
    }

private:
    const int id_;
    UartConfig cfg_;
    int& txCounter_;
    bool open_ = false;     // NSDMI: važi za svaki konstruktor koji ga ne postavi
};

int main() {
    UartConfig a{};
    UartConfig b{115200};
    UartConfig c{115200, 'E', 2};
    for (const UartConfig* p : {&a, &b, &c})
        std::cout << p->baud << ' ' << p->parity << ' ' << p->stopBits << '\n';

    int sent = 0;
    Uart u(3, c, sent);
    u.send("pre open");
    u.open();
    u.send("a");
    u.send("b");
    u.print();
    std::cout << "brojač kod pozivaoca: " << sent << '\n';
}
