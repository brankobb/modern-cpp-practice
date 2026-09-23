// EXPECT-GCC: std::thread arguments must be invocable after conversion to rvalues
// EXPECT-CLANG: std::thread arguments must be invocable after conversion to rvalues
// POGREŠNO: std::thread KOPIRA argumente u sopstveni prostor nove niti i
// predaje ih funkciji kao rvalue (privremene vrednosti). Parametar int&
// ne može da se veže za rvalue -- i dobro je da ne može: menjao bi
// kopiju, a ne brojac.
// Ispravno: std::thread t(uvecaj, std::ref(brojac));
// (i onda brojac mora da živi dok nit radi -- ovde je join pre return-a).
#include <thread>
void uvecaj(int& x) { ++x; }
int main() {
    int brojac = 0;
    std::thread t(uvecaj, brojac);
    t.join();
    return brojac;
}
