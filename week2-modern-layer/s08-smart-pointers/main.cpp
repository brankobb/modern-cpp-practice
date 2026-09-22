#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

// Vežba A: unique_ptr sa custom deleter-om (npr. za FILE* ili sopstveni
// "resurs" sa custom cleanup funkcijom umesto delete).
//
// Vežba B: napravi kružnu referencu sa dva shared_ptr (A drži shared_ptr<B>,
// B drži shared_ptr<A>) -- pokaži da se objekti NIKAD ne oslobađaju (leak).
// Zatim popravi tako da jedan smer bude weak_ptr, i pokaži da se sada
// oslobađaju.

struct Node {
    std::string name;
    // Ako OBA smera veze (next napred i nazad) drže shared_ptr (NIJE
    // DOBRO) jer se pravi CIKLUS -- brojač referenci nikad ne padne na 0
    // ni za A ni za B (svaki drži onaj drugi "živim"), pa se destruktori
    // NIKAD ne pozivaju čak i kad oba izađu iz scope-a u main-u (proveri:
    // da li vidiš "~Node" ispis na kraju programa?).
    // Treba da BAR JEDAN smer bude weak_ptr -- weak_ptr NE UTIČE na
    // brojač referenci, samo "posmatra" objekat i može bezbedno da
    // proveri da li još postoji (preko lock()).
    // Možeš i izbeći ciklus u dizajnu -- npr. da "nazad" veza uopšte ne
    // postoji, ili koristiti sirov pokazivač za "ne-vlasnički" smer ako
    // si siguran da će owner uvek nadživeti tu referencu.
    std::shared_ptr<Node> next;     // TODO: probaj weak_ptr ovde da razbiješ ciklus
    ~Node() { std::cout << "~Node(" << name << ")\n"; }
};

int main() {
    std::cout << "-- unique_ptr sa custom deleter --\n";
    auto file_closer = [](FILE* f) {
        if (f) std::fclose(f);
    };
    std::unique_ptr<FILE, decltype(file_closer)> fp(nullptr, file_closer);

    std::cout << "-- shared_ptr ciklus (namerni leak) --\n";
    auto a = std::make_shared<Node>();
    auto b = std::make_shared<Node>();
    a->name = "A";
    b->name = "B";
    a->next = b;
    b->next = a; // ciklus -- ni a ni b se ne oslobađaju bez weak_ptr
    std::cout << "main se zavrsava -- proveri da li se ~Node poziva ispod ovog reda\n";
}
