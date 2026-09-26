// KIND: why
// DEMO-UB: NAIVE bad-free|attempting double-free
//
// Zadatak 3 -- zašto enable_shared_from_this, a ne shared_ptr(this)
// (sekcija 8)
// Rešenje: exercises/solutions/ex3_shared_from_this.cpp
//
// Session se registruje u registru (registry), koji čuva shared_ptr na nju (da
// sesija živi dok je registrovana).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 6-smart-pointers/32-smart-pointers/exercises/ex3_shared_from_this.cpp -DNAIVE
//   std::shared_ptr<Session>(this) pravi NOVI kontrolni blok sa sopstvenim
//   brojačem -- ne zna za shared_ptr u main-u. Dva nezavisna brojača, oba
//   padnu na 0, dva brisanja istog objekta. ASan prijavi "attempting free
//   on address which was not malloc()-ed" (bad-free): make_shared je
//   objekat smestio UNUTAR bloka koji deli sa brojačem, pa delete na
//   adresi objekta nije ni početak alokacije. Da je s napravljen sa
//   std::shared_ptr<Session>(new Session), prijava bi bila
//   "attempting double-free" (provereno).
// Korak 2: u #else grani: Session nasleđuje
//   std::enable_shared_from_this<Session>, a enroll() koristi
//   shared_from_this() -- vraća shared_ptr koji deli POSTOJEĆI kontrolni
//   blok.
// Korak 3: shared_from_this() radi samo ako objekat već ima vlasnika
//   (shared_ptr). Za objekat na steku, od C++17 baca std::bad_weak_ptr --
//   probaj u try/catch.

#include <iostream>
#include <memory>
#include <vector>

#ifdef NAIVE
struct Session;
std::vector<std::shared_ptr<Session>> registry;

struct Session {
    void enroll() { registry.push_back(std::shared_ptr<Session>(this)); }   // drugi brojač!
};

int main() {
    {
        auto s = std::make_shared<Session>();
        s->enroll();
        std::cout << "use_count: " << s.use_count() << '\n';   // 1, a ne 2
    }
    registry.clear();
}
#else
// TODO korak 2

int main() {
    // Korak 2 -- otkomentariši:
    // {
    //     auto s = std::make_shared<Session>();
    //     s->enroll();
    //     std::cout << "use_count: " << s.use_count() << '\n';
    // }
    // std::cout << "posle bloka, u registru: " << registar.size() << ", use_count: "
    //           << registry[0].use_count() << '\n';
    // registry.clear();

    // Korak 3 -- otkomentariši:
    // Session onStack;
    // try {
    //     onStack.enroll();
    // } catch (const std::bad_weak_ptr&) {
    //     std::cout << "on the stack: bad_weak_ptr\n";
    // }
}
#endif

/* EXPECTED OUTPUT
use_count: 2
after the block, in the registry: 1, use_count: 1
on the stack: bad_weak_ptr
*/
