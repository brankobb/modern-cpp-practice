// KIND: why
// DEMO-UB: NAIVE detected memory leaks
//
// Zadatak 2 -- zašto weak_ptr za "pokazivač nazad" (sekcija 5)
// Rešenje: exercises/solutions/ex2_circular_reference.cpp
//
// Node (čvor stabla) drži decu (shared_ptr), a dete pamti roditelja.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 6-smart-pointers/32-smart-pointers/exercises/ex2_circular_reference.cpp -DNAIVE
//   Nijedan destruktor se ne pozove, a LeakSanitizer prijavi curenje.
//   Posle kraja bloka koren i dete drže jedan drugog: svaki ima
//   use_count 1, pa nijedan ne pada na 0. shared_ptr broji vlasnike, ne
//   traži cikluse.
// Korak 2: u #else grani napiši Node gde je roditelj
//   std::weak_ptr<Node> -- dete POSMATRA roditelja, ne poseduje ga.
//   Vlasništvo ide samo nadole (roditelj -> deca). Metoda
//   std::string parentName() const vraća ime roditelja preko lock(), ili
//   "(nema)".

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#ifdef NAIVE
struct Node {
    explicit Node(std::string i) : name(std::move(i)) {}
    ~Node() { std::cout << "~Node(" << name << ")\n"; }
    std::string name;
    std::shared_ptr<Node> parent;                  // vlasništvo nagore: ciklus
    std::vector<std::shared_ptr<Node>> children;
};

int main() {
    // Tri stabla, a ne jedno: LeakSanitizer je konzervativan -- zaostala
    // kopija pokazivača na steku (od već uništenog shared_ptr-a) može da
    // mu "sakrije" poslednje stablo. Sa clang-om se to dešava otprilike u
    // pola pokretanja; prva dva stabla prijavi uvek (provereno). Isto radi
    // ub/u01_shared_ptr_cycle.
    for (int i = 0; i < 3; ++i) {
        auto root = std::make_shared<Node>("root");
        auto child = std::make_shared<Node>("child");
        child->parent = root;
        root->children.push_back(child);
        if (i == 0) std::cout << "root use_count: " << root.use_count() << '\n';
    }
    std::cout << "end of block\n";
}
#else
// TODO korak 2

int main() {
    // Korak 2 -- otkomentariši:
    // {
    //     auto root = std::make_shared<Node>("root");
    //     auto child = std::make_shared<Node>("child");
    //     child->parent = root;
    //     root->children.push_back(child);
    //     std::cout << "root use_count: " << root.use_count() << '\n';
    //     std::cout << "parent of the child: " << child->parentName() << '\n';
    // }
    // std::cout << "end of block\n";
}
#endif

/* EXPECTED OUTPUT
root use_count: 1
parent of the child: root
~Node(root)
~Node(child)
end of block
*/
