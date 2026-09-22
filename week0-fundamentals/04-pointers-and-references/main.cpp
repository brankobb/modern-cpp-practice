#include <iostream>

void arrayDecayTrap(int arr[]) {
    // arr je ovde POKAZIVAČ, ne niz -- sizeof daje 8 (na 64-bit), ne 5*4
    std::cout << "sizeof(arr) unutar funkcije = " << sizeof(arr) << "\n";
}

int* danglingPointer() {
    int local = 42;
    return &local; // BUG: vraća adresu lokalne promenljive koja izlazi iz scope-a
}

void referenceBasics() {
    int x = 10;
    int& ref = x; // MORA se inicijalizovati ovde
    ref = 20;      // menja x, jer je ref alias za x
    std::cout << "x = " << x << " (posle ref = 20)\n";

    int y = 99;
    // ref = y;    // OVO NE "rebinduje" ref na y -- ovo je ASSIGNMENT, kopira 99 u x!
    // ref sada i dalje referiše x, samo je x sad 99
}

int main() {
    int arr[5] = {1, 2, 3, 4, 5};
    std::cout << "sizeof(arr) u main = " << sizeof(arr) << " (ceo niz)\n";
    arrayDecayTrap(arr);

    referenceBasics();

    int* dangling = danglingPointer();
    std::cout << "dereferenciranje danglinga (UB, ASan treba da uhvati): ";
    std::cout << *dangling << "\n"; // pokreni pod ASan-om
}
