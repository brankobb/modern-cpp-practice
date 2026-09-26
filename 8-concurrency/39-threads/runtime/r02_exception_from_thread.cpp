// EXPECT-RUN: terminate called after throwing an instance of 'std::runtime_error'
// POGREŠNO: izuzetak koji napusti funkciju niti poziva std::terminate.
// join ga NE prenosi u nit koja čeka -- svaka nit ima svoj stek, i
// try/catch oko join-a ne bi pomogao.
// Ispravno: uhvati izuzetak u samoj niti; da bi ga dobio pozivalac:
// std::async / std::promise + future.get() (lekcija 40, sekcija 5).
#include <stdexcept>
#include <thread>
void work() { throw std::runtime_error("sensor not responding"); }
int main() {
    std::thread t(work);
    t.join();
}
