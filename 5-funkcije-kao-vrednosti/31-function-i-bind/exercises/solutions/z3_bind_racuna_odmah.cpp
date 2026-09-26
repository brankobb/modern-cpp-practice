// Rešenje zadatka z3_bind_racuna_odmah.

#include <functional>
#include <iostream>

int sat = 8;
int trenutnoVreme() { return sat; }
void postaviAlarm(int kada) { std::cout << "alarm postavljen na " << kada << "h\n"; }

int main() {
    // Ako vreme prosleđuješ kao argument bind-a (nije dobro): izraz se
    // izračuna jednom, kad se pravi callback.
    // Treba ovako: lambda -- telo se izvršava pri svakom pozivu, pa se
    // vreme uzme kad je dugme pritisnuto.
    std::function<void()> odlozi = [] { postaviAlarm(trenutnoVreme() + 1); };
    sat = 12;
    odlozi();
}
