// EXPECT-UB: stack-use-after-return
// UB: posle detach() nit radi sama, a niko ne čeka njen kraj. Lambda je
// zarobila REFERENCU na lokalnu promenljivu funkcije pokreni(); pokreni()
// se vrati odmah, a nit 50 ms kasnije čita memoriju steka koji više ne
// postoji. (g++ 13 ASan ovo hvata podrazumevano; sleep u main-u samo da
// program ne završi pre niti.)
// Ispravno: join umesto detach; ili zarobi po vrednosti ([lokalna]);
// detach samo kad nit ne koristi ništa što pripada pozivaocu.
#include <chrono>
#include <iostream>
#include <thread>
void pokreni() {
    int lokalna = 5;
    std::thread t([&lokalna] {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::cout << lokalna << '\n';
    });
    t.detach();
}
int main() {
    pokreni();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
