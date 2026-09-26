// Rešenje zadatka ex3_bind_evaluates_now.

#include <functional>
#include <iostream>

int hour = 8;
int currentTime() { return hour; }
void setAlarm(int when) { std::cout << "alarm set to " << when << "h\n"; }

int main() {
    // Ako vreme prosleđuješ kao argument bind-a (nije dobro): izraz se
    // izračuna jednom, kad se pravi callback.
    // Treba ovako: lambda -- telo se izvršava pri svakom pozivu, pa se
    // vreme uzme kad je dugme pritisnuto.
    std::function<void()> snooze = [] { setAlarm(currentTime() + 1); };
    hour = 12;
    snooze();
}
