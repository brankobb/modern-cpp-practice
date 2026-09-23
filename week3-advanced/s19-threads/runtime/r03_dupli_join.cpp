// EXPECT-RUN: terminate called after throwing an instance of 'std::system_error'
// POGREŠNO: posle join() objekat više nije vezan ni za jednu nit
// (joinable() == false). Drugi join baca std::system_error
// ("Invalid argument"); neuhvaćen -- terminate.
// Ispravno: if (t.joinable()) t.join();
#include <thread>
void posao() {}
int main() {
    std::thread t(posao);
    t.join();
    t.join();
}
