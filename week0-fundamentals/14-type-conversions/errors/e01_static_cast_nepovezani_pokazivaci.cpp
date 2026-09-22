// STD: c++17
// EXPECT-GCC: invalid 'static_cast' from type 'Celsius*' to type 'Account*'
// EXPECT-CLANG: which are not related by inheritance, is not allowed
// POGREŠNO: static_cast između pokazivača na nepovezane tipove.
// Zašto: static_cast dozvoljava samo konverzije koje imaju smisla po
//   pravilima jezika: brojevi, enum, void* <-> T*, i gore/dole kroz
//   NASLEĐIVANJE. Celsius i Account nemaju vezu, pa bi rezultat pokazivao
//   na objekat pogrešnog tipa. C-cast "(Account*)&c" bi se TIHO kompajlirao
//   (kao reinterpret_cast) -- zato se C-cast ne koristi (ES.48, ES.49).
// Ispravno: pravi konverziju (funkcija ili konstruktor), a ne cast pokazivača.
struct Celsius {
    double v;
};
struct Account {
    long cents;
};

int main() {
    Celsius c{1};
    Account* a = static_cast<Account*>(&c);
    return static_cast<int>(a->cents);
}
