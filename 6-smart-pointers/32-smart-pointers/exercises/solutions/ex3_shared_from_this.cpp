// Rešenje zadatka ex3_shared_from_this.

#include <iostream>
#include <memory>
#include <vector>

struct Session;
std::vector<std::shared_ptr<Session>> registry;

// Ako praviš std::shared_ptr<Session>(this) (nije dobro): drugi, nezavisan
// kontrolni blok -- objekat se obriše dvaput.
// Treba ovako: enable_shared_from_this u sebi čuva weak_ptr na postojeći
// kontrolni blok (popuni ga make_shared / shared_ptr konstruktor), a
// shared_from_this() od njega napravi shared_ptr.
struct Session : std::enable_shared_from_this<Session> {
    void enroll() { registry.push_back(shared_from_this()); }
};

int main() {
    {
        auto s = std::make_shared<Session>();
        s->enroll();
        std::cout << "use_count: " << s.use_count() << '\n';
    }
    std::cout << "after the block, in the registry: " << registry.size() << ", use_count: "
              << registry[0].use_count() << '\n';
    registry.clear();

    // Korak 3: bez vlasnika weak_ptr je prazan, pa shared_from_this() baca
    // (C++17; ranije je bilo UB).
    Session onStack;
    try {
        onStack.enroll();
    } catch (const std::bad_weak_ptr&) {
        std::cout << "on the stack: bad_weak_ptr\n";
    }
}
