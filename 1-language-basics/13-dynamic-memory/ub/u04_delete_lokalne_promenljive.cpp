// EXPECT-UB: attempting free on address which was not malloc\(\)-ed
// POGREŠNO: delete na adresi lokalne promenljive.
// Zašto: delete sme samo na pokazivač dobijen od new (ili nullptr). Lokalna
//   promenljiva je na steku i nestaje sama; alokator o njoj ne zna ništa.
//   Česta varijanta: funkcija prima T* i ne zna da li je objekat na heap-u,
//   pa ga "za svaki slučaj" obriše. Zato sirov pokazivač ne sme da znači
//   vlasništvo (lekcija 04, R.3). g++ -Wall ovde upozorava (-Wfree-nonheap-object).
// Ispravno: ne briši ono što nisi alocirao sa new.
int main() {
    int x = 5;
    int* p = &x;
    delete p;
}
