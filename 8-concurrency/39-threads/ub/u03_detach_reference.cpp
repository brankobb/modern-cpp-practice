// EXPECT-UB: stack-use-after-return
// UB: posle detach() nit radi sama, a niko ne čeka njen kraj. Lambda je
// zarobila REFERENCU na lokalnu promenljivu funkcije pokreni(); pokreni()
// se vrati odmah, a nit 50 ms kasnije čita memoriju steka koji više ne
// postoji. (g++ 13 ASan ovo hvata podrazumevano; sleep u main-u samo da
// program ne završi pre niti.)
// Ispravno: join umesto detach; ili zarobi po vrednosti ([local]);
// detach samo kad nit ne koristi ništa što pripada pozivaocu.
#include <chrono>
#include <iostream>
#include <thread>
void launch() {
    int local = 5;
    std::thread t([&local] {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::cout << local << '\n';
    });
    t.detach();
}
int main() {
    launch();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
