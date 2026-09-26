// EXPECT-GCC: use of deleted function 'std::thread::thread(const std::thread&)'
// EXPECT-CLANG: call to deleted constructor of 'std::thread'
// POGREŠNO: std::thread predstavlja JEDNU nit izvršavanja -- dva objekta
// ne mogu da je "poseduju" (ko bi radio join?). Kopiranje je obrisano,
// kao kod unique_ptr.
// Ispravno: premesti vlasništvo: std::thread drugi = std::move(t);
// (posle toga t.joinable() == false).
#include <thread>
void posao() {}
int main() {
    std::thread t(posao);
    std::thread kopija = t;
    kopija.join();
}
