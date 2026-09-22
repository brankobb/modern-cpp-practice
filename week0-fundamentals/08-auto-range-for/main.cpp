#include <iostream>
#include <vector>

void autoDropsConstRef() {
    std::cout << "-- autoDropsConstRef --\n";
    int x = 5;
    const int& cref = x;
    // Ako pretpostaviš da auto ČUVA const/referencu (NIJE DOBRO, greška u
    // razmišljanju) jer auto SKIDA top-level const i referencu
    // podrazumevano -- y ispod je obican int, kopija, ne const int&.
    auto y = cref;       // y je int (NE const int&) -- moze da se menja
    // Treba da napišeš auto& ili const auto& EKSPLICITNO kad ti treba da
    // zadržiš referencu/const (npr. da izbegneš kopiranje velikog
    // objekta, ili da spreciš izmenu).
    // Možeš i decltype(cref) ako ti baš treba TAČAN tip izraza
    // (uključujući const i referencu) -- ređe potrebno u praksi.
    y = 99;
    std::cout << "x=" << x << " y=" << y << " (x nepromenjen jer je y kopija)\n";
}

struct Item {
    int value;
    void doubleIt() { value *= 2; }
};

void rangeForByValueTrap() {
    std::vector<Item> items = {{1}, {2}, {3}};

    std::cout << "-- range-for sa 'auto item' (kopija) --\n";
    // Ako pišeš for (auto item : items) kad namerno menjaš elemente (NIJE
    // DOBRO) jer auto (bez &) pravi KOPIJU svakog elementa -- izmene se
    // dešavaju na kopiji, original ostaje netaknut.
    for (auto item : items) { // BUG (namerno): ovo je kopija!
        item.doubleIt();
    }
    std::cout << "posle 'auto item' petlje: ";
    for (const auto& item : items) std::cout << item.value << " ";
    std::cout << "(nepromenjeno -- radili smo na kopijama)\n";

    std::cout << "-- range-for sa 'auto& item' (referenca) --\n";
    // Treba da koristiš auto& kad ŽELIŠ da menjaš originalne elemente
    // kroz petlju -- referenca "gleda" na stvarni element u kontejneru.
    for (auto& item : items) { // ISPRAVNO: referenca menja original
        item.doubleIt();
    }
    std::cout << "posle 'auto& item' petlje: ";
    for (const auto& item : items) std::cout << item.value << " ";
    std::cout << "(sad JESTE promenjeno)\n";
    // Možeš i const auto& kad SAMO ČITAŠ elemente (ne menjaš ih) --
    // izbegava kopiranje bez rizika slučajne izmene; ovo je DEFAULT
    // izbor za read-only iteraciju (koristimo ga gore za ispis).
}

int main() {
    autoDropsConstRef();
    rangeForByValueTrap();
}
