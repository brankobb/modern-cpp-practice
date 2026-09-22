#include <iostream>
#include <vector>

struct Widget {
    Widget() { std::cout << "Widget()\n"; }
    Widget(int) { std::cout << "Widget(int)\n"; }
    Widget(std::initializer_list<int>) { std::cout << "Widget(initializer_list)\n"; }
};

struct ExplicitOnly {
    explicit ExplicitOnly(int) { std::cout << "ExplicitOnly(int)\n"; }
};

int main() {
    std::cout << "-- narrowing: () vs {} --\n";
    // Ako inicijalizuješ sa {} kad bi se izgubila preciznost (NIJE
    // DOBRO, i kompajler te zato zaustavlja): int narrow{3.14}; -- NE
    // kompajlira, {} zabranjuje narrowing conversion.
    // int narrow{3.14}; // TODO: otkomentariši -- treba compile error

    // Treba da koristiš {} BAŠ ZATO da uhvatiš ovakve greške na
    // kompajliranju, umesto da otkriješ tihi gubitak podataka tek u
    // runtime-u (kao ispod).
    // Možeš i koristiti () ili = kad namerno želiš konverziju (npr.
    // eksplicitno seciranje na ceo broj) -- ali onda dodaj
    // static_cast<int>(3.14) da bude jasno da je to namera, ne slučajnost.
    int narrow_ok(3.14); // ovo prolazi (uz warning) -- uporedi sa gornjim
    std::cout << "narrow_ok = " << narrow_ok << "\n";

    std::cout << "-- most vexing parse --\n";
    // Ako napišeš Widget w1(); misleći da praviš objekat (NIJE DOBRO) jer
    // kompajler ovo čita kao DEKLARACIJU FUNKCIJE koja se zove w1, ne
    // prima ništa i vraća Widget -- objekat se NIKAD ne konstruiše.
    Widget w1();  // TODO: ovo je funkcija koja vraća Widget, NE objekat!
                  // proveri: std::cout << typeid(w1).name(); ne kompajlira jer w1 je funkcija

    // Treba da koristiš {} kad želiš default-konstruisan objekat -- {}
    // nema tu dvosmislenost sa deklaracijom funkcije.
    Widget w2{};  // ovo JESTE default-konstruisan objekat

    std::cout << "-- initializer_list preferencija --\n";
    Widget w3(5);   // poziva Widget(int)
    // Ako klasa ima i Widget(int) i Widget(initializer_list<int>), a ti
    // pozoveš sa {} (MOŽE BITI IZNENAĐENJE) jer {} UVEK preferira
    // initializer_list ctor ako postoji, čak i kad si očigledno mislio na
    // Widget(int).
    Widget w4{5};   // poziva Widget(initializer_list) jer postoji -- iznenađenje!
    // Treba da budeš svestan ovog pravila kad dizajniraš sopstvenu klasu
    // sa oba ctor-a -- ili izbegavaj da imaš oba, ili jasno dokumentuj
    // koji {} bira.
    // Možeš i eksplicitno pozvati Widget w4(5); sa () kad želiš BAŠ
    // Widget(int) a ne initializer_list verziju.

    std::cout << "-- vector(3, 5) vs vector{3, 5} --\n";
    // klasični primer: (3, 5) vs {3, 5} -- ista zamka kao gore, na klasi
    // koju svakodnevno koristiš.
    std::vector<int> va(3, 5); // 3 elementa, svaki = 5 -> [5, 5, 5]
    std::vector<int> vb{3, 5}; // initializer_list -> [3, 5]
    std::cout << "va.size()=" << va.size() << " vb.size()=" << vb.size() << "\n";

    std::cout << "-- direct-init vs copy-init (explicit) --\n";
    // Ako pozoveš explicit ctor preko copy-initialization sintakse (=)
    // (NIJE DOBRO, ne kompajlira) jer je explicit BAŠ ZATO tu -- da
    // ISKLJUČI konstruktor iz razmatranja kod copy-init (i copy-list-init),
    // ne dozvoljava "tihu" konverziju kroz =.
    // ExplicitOnly bad1 = 5;    // TODO: otkomentariši -- compile error (copy-init)
    // ExplicitOnly bad2 = {5};  // TODO: otkomentariši -- compile error (copy-list-init, ista zamka)

    // Treba da koristiš direct-initialization ((), ili {} BEZ =) kad
    // pozivaš explicit ctor -- direct-init I direct-list-init GA
    // razmatraju, jer nema implicitne konverzije, poziv je nedvosmisleno
    // namerni.
    ExplicitOnly good1(5);  // direct-init -- radi
    ExplicitOnly good2{5};  // direct-list-init -- radi
    // Možeš i eksplicitno napisati konverziju na mestu poziva
    // (ExplicitOnly good3 = ExplicitOnly(5);) kad ti STVARNO treba
    // copy-init sintaksa (npr. API koji to zahteva) -- to je i dalje
    // direct-init "iznutra", = samo kopira/premešta već napravljen objekat.
    (void)good1;
    (void)good2;

    (void)narrow_ok;
    (void)w3;
    (void)w4;
}
