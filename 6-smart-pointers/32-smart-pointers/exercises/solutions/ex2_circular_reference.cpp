// Rešenje zadatka ex2_circular_reference.

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Ako i roditelj i dete drže shared_ptr jedan na drugog (nije dobro):
// ciklus -- brojači nikad ne padnu na 0, niko se ne uništi.
// Treba ovako: vlasništvo samo u jednom smeru (roditelj poseduje decu), a
// pokazivač nazad je weak_ptr.
struct Node {
    explicit Node(std::string i) : name(std::move(i)) {}
    ~Node() { std::cout << "~Node(" << name << ")\n"; }
    std::string name;
    std::weak_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;

    std::string parentName() const {
        if (auto r = parent.lock()) return r->name;
        return "(none)";
    }
};

int main() {
    {
        auto root = std::make_shared<Node>("root");
        auto child = std::make_shared<Node>("child");
        child->parent = root;
        root->children.push_back(child);
        std::cout << "root use_count: " << root.use_count() << '\n';
        std::cout << "parent of the child: " << child->parentName() << '\n';
    }
    // Redosled: lokalne se uništavaju obrnuto -- prvo "child" (use_count
    // deteta pada na 1, drži ga root), pa "root" (pada na 0): ~Node(root),
    // a u njegovom destruktoru vektor dece pusti child -> ~Node(child).
    std::cout << "end of block\n";
}
