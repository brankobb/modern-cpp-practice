#include <iostream>
#include <memory>

// Vežba A: unique_ptr sa custom deleter-om (npr. za FILE* ili sopstveni
// "resurs" sa custom cleanup funkcijom umesto delete).
//
// Vežba B: napravi kružnu referencu sa dva shared_ptr (A drži shared_ptr<B>,
// B drži shared_ptr<A>) -- pokaži da se objekti NIKAD ne oslobađaju (leak).
// Zatim popravi tako da jedan smer bude weak_ptr, i pokaži da se sada
// oslobađaju.

struct Node {
    std::string name;
    std::shared_ptr<Node> next;     // TODO: probaj weak_ptr ovde da razbiješ ciklus
    ~Node() { std::cout << "~Node(" << name << ")\n"; }
};

int main() {
    // unique_ptr sa custom deleter
    auto file_closer = [](FILE* f) {
        if (f) std::fclose(f);
    };
    std::unique_ptr<FILE, decltype(file_closer)> fp(nullptr, file_closer);

    // kružna referenca (leak dok je next shared_ptr)
    auto a = std::make_shared<Node>();
    auto b = std::make_shared<Node>();
    a->name = "A";
    b->name = "B";
    a->next = b;
    b->next = a; // ciklus -- ni a ni b se ne oslobađaju bez weak_ptr
}
