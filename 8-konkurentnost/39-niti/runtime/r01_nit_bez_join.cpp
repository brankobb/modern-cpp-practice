// EXPECT-RUN: terminate called without an active exception
// POGREŠNO: destruktor std::thread-a koji je još joinable (nije ni join
// ni detach) poziva std::terminate ([thread.thread.destr]). Standard ne
// bira umesto tebe: tihi join bi mogao da zaglavi, tihi detach da ostavi
// nit sa visećim referencama.
// Ispravno: t.join() pre kraja opsega. Pazi na izuzetak između
// pravljenja niti i join-a -- tada join nikad ne dođe (RAII omotač, ili
// C++20 std::jthread, koji u destruktoru sam pozove join).
#include <thread>
void posao() {}
int main() {
    std::thread t(posao);
}
