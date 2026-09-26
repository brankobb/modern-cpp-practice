// EXPECT-UB: does not point to an object of type 'Dog'
// POGREŠNO: static_cast<Dog&> na Animal& koji je zapravo Cat.
// Zašto: static_cast naniže (bazna -> izvedena) ne proverava pri
//   izvršavanju; kompajler veruje da je objekat zaista Dog. Ovde je Cat, pa
//   d.volume čita memoriju koja nije Dog. UBSan (-fsanitize=vptr, deo
//   -fsanitize=undefined u g++) proverava tip preko vptr-a.
// Ispravno: dynamic_cast<Dog*>(&a) vraća nullptr ako objekat nije Dog
//   (lekcija 17). Još bolje: virtual funkcija umesto cast-a.
#include <cstdio>

struct Animal {
    virtual ~Animal() = default;
};
struct Dog : Animal {
    int volume = 5;
};
struct Cat : Animal {
    int lives = 9;
};

int main() {
    Cat c;
    Animal& a = c;
    Dog& d = static_cast<Dog&>(a);
    std::printf("%d\n", d.volume);
}
