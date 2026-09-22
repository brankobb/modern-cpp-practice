#include <iostream>

void arrayDecayTrap(int arr[]) {
    // Ako se osloniš na sizeof(arr) unutar funkcije da saznaš dužinu niza
    // (NIJE DOBRO) jer je niz "decay-ovao" u pokazivač čim je prosleđen --
    // dobićeš veličinu POKAZIVAČA (8 bajtova na 64-bit), ne veličinu
    // originalnog niza.
    // Treba da prosledis dužinu EKSPLICITNO kao poseban parametar
    // (void f(int* arr, size_t n)), ili koristiš std::array/std::vector
    // koji nose svoju veličinu sa sobom.
    // Možeš i proslediti referencu na niz FIKSNE veličine
    // (void f(int (&arr)[5])) -- tad sizeof radi ispravno, ali funkcija
    // onda radi SAMO za nizove te tačne veličine.
    std::cout << "sizeof(arr) unutar funkcije = " << sizeof(arr) << "\n";
}

int* danglingPointer() {
    int local = 42;
    // Ako vratiš adresu lokalne promenljive (NIJE DOBRO) jer local izlazi
    // iz scope-a čim se funkcija završi -- pokazivač koji vraćaš pokazuje
    // na memoriju koja više "nije tvoja" (stack frame je uništen).
    // Treba da vratiš PO VREDNOSTI (return local;) ako želiš kopiju, ili
    // alociraš na heap-u (new int(42)) ako objekat mora da preživi poziv
    // funkcije -- ali onda si ti odgovoran za delete.
    // Možeš i koristiti static lokalnu promenljivu (static int local =
    // 42;) ako ti treba da "preživi" pozive, ali onda je DELJENA između
    // svih poziva funkcije -- retko je to ono što stvarno želiš.
    return &local; // BUG: vraća adresu lokalne promenljive koja izlazi iz scope-a
}

void referenceBasics() {
    std::cout << "-- referenceBasics --\n";
    int x = 10;
    int& ref = x; // MORA se inicijalizovati ovde
    ref = 20;      // menja x, jer je ref alias za x
    std::cout << "x = " << x << " (posle ref = 20)\n";

    int y = 99;
    // Ako pokušaš ref = y; misleći da REBINDUJEŠ ref na y (NIJE DOBRO, to
    // ne radi to) jer se reference u C++-u NE MOGU rebindovati posle
    // inicijalizacije -- ovo je ASSIGNMENT, kopira vrednost y (99) u x
    // (na koji ref i dalje pokazuje).
    // Treba da koristiš POKAZIVAČ ako ti treba promenljiva koja može da
    // "promeni metu" tokom vremena (int* p = &x; p = &y;).
    // Možeš i napraviti NOVU referencu na y ako ti to zapravo treba (int&
    // ref2 = y;) -- ref i ref2 su dve odvojene, nezavisne reference.
    // ref = y;    // OVO NE "rebinduje" ref na y -- ovo je ASSIGNMENT, kopira 99 u x!
    // ref sada i dalje referiše x, samo je x sad 99
}

int main() {
    std::cout << "-- arrayDecayTrap --\n";
    int arr[5] = {1, 2, 3, 4, 5};
    std::cout << "sizeof(arr) u main = " << sizeof(arr) << " (ceo niz)\n";
    arrayDecayTrap(arr);

    referenceBasics();

    std::cout << "-- danglingPointer (namerni bag, ASan treba da uhvati) --\n";
    int* dangling = danglingPointer();
    std::cout << "dereferenciranje danglinga (UB, ASan treba da uhvati): ";
    std::cout << *dangling << "\n"; // pokreni pod ASan-om
}
