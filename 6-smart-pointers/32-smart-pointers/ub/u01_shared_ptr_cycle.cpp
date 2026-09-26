// EXPECT-UB: LeakSanitizer: detected memory leaks
// POGREŠNO: dva objekta drže shared_ptr jedan na drugi.
// Zašto: kad a i b izađu iz scope-a, svaki objekat i dalje ima jednog
//   vlasnika -- onog drugog. use_count nikad ne padne na 0, pa se ~Node
//   nikad ne pozove (u ispisu nema "~Node"), a memorija ostane zauzeta.
//   Ovo nije UB nego curenje; LeakSanitizer ga prijavi na kraju programa.
// Ispravno: bar jedan smer weak_ptr (main.cpp, sekcija 5). U stablu:
//   roditelj drži decu (shared_ptr/unique_ptr), dete roditelja weak_ptr-om
//   ili sirovim pokazivačem.
#include <cstdio>
#include <memory>
#include <string>

struct Node {
    explicit Node(std::string n) : name(std::move(n)) {}
    ~Node() { std::printf("~Node(%s)\n", name.c_str()); }
    std::string name;
    std::shared_ptr<Node> partner;
};

int main() {
    for (int i = 0; i < 3; ++i) {
        auto a = std::make_shared<Node>("A");
        auto b = std::make_shared<Node>("B");
        a->partner = b;
        b->partner = a;
    }
    std::printf("end of main\n");
}
