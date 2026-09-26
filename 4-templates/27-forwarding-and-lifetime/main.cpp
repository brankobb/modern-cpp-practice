#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

// Lekcija 27 -- forwarding reference, std::forward i zamke životnog veka:
// ISPRAVNI slučajevi. Sve se kompajlira i radi bez ASan/UBSan prijava
// (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 4-templates/27-forwarding-and-lifetime  proverava oba.

// ---------------------------------------------------------------- 1
template <typename T>
const char* deducedT() { // šta je T dedukovan za argument tipa std::string
    if (std::is_same_v<T, std::string&>) return "std::string&";
    if (std::is_same_v<T, const std::string&>) return "const std::string&";
    if (std::is_same_v<T, std::string>) return "std::string";
    return "?";
}

template <typename T>
void universal(T&&) { std::cout << "T=" << deducedT<T>(); } // T&& + dedukcija = forwarding referenca

void rvalueOnly(std::string&&) {} // konkretan tip: obična rvalue referenca

template <typename T>
void vectorRvalue(std::vector<T>&&) {} // T&& je deo drugog tipa: NIJE forwarding (errors/e02)

void s01_universalReferences() {
    std::cout << "-- 1. forwarding (universal) vs rvalue reference (EMC Item 24) --\n";
    std::string name = "Ann";
    const std::string constName = "Bob";
    std::cout << "  universal(name) -> ";
    universal(name);
    std::cout << "; universal(constName) -> ";
    universal(constName);
    std::cout << "; universal(std::string(\"x\")) -> ";
    universal(std::string("x"));
    std::cout << "\n";
    rvalueOnly(std::string("ok")); // rvalueOnly(name) se ne bi kompajliralo
    vectorRvalue(std::vector<int>{1});
    auto&& bound = name; // auto&& je takođe forwarding: ovde postaje std::string&
    std::cout << "  auto&& bound = name -> " << (std::is_lvalue_reference_v<decltype(bound)> ? "lvalue reference" : "rvalue reference")
              << "\n";
}

// ---------------------------------------------------------------- 2
template <typename T>
using AddRvalueRef = T&&; // T&& kad je T već referenca -> "reference collapsing"

void s02_referenceCollapsing() {
    std::cout << "-- 2. reference collapsing (EMC Item 28) --\n";
    static_assert(std::is_same_v<AddRvalueRef<int&>, int&>, "& && -> &");
    static_assert(std::is_same_v<AddRvalueRef<int&&>, int&&>, "&& && -> &&");
    static_assert(std::is_same_v<AddRvalueRef<int>, int&&>, "T -> T&&");
    std::cout << "  T=int& -> int&;  T=int&& -> int&&;  T=int -> int&&  (checked with static_assert)\n";
    std::cout << "  <- if there is a & anywhere, the result is &. That is why T&& with T = std::string& becomes std::string&\n";
}

// ---------------------------------------------------------------- 3
const char* inner(const std::string&) { return "inner(const&)"; }
const char* inner(std::string&&) { return "inner(&&)"; }

template <typename T>
const char* withoutForward(T&& arg) { return inner(arg); } // arg ima ime -> uvek lvalue

template <typename T>
const char* withForward(T&& arg) { return inner(std::forward<T>(arg)); } // vrati originalnu kategoriju

void s03_forward() {
    std::cout << "-- 3. std::forward preserves the category --\n";
    std::string s = "x";
    std::cout << "  without forward: lvalue -> " << withoutForward(s) << ", rvalue -> " << withoutForward(std::string("y")) << "\n";
    std::cout << "  with forward:    lvalue -> " << withForward(s) << ", rvalue -> " << withForward(std::string("y")) << "\n";
}

// ---------------------------------------------------------------- 4
class Profile {
public:
    // NE RADI OVAKO: std::move na forwarding referenci pomera i LVALUE argumente.
    template <typename T>
    void setNameMove(T&& name) { name_ = std::move(name); }

    // ✅ std::forward: pomera samo ono što je pozivalac dao kao rvalue (EMC Item 25).
    template <typename T>
    void setNameForward(T&& name) { name_ = std::forward<T>(name); }

private:
    std::string name_;
};

void s04_moveVsForward() {
    std::cout << "-- 4. std::move vs std::forward on a forwarding reference (EMC Item 25) --\n";
    Profile p;
    std::string mine(30, 'a');
    p.setNameMove(mine);
    std::cout << "  setNameMove(mine):    mine.size() after = " << mine.size() << "  <- stolen from the caller!\n";
    std::string other(30, 'b');
    p.setNameForward(other);
    std::cout << "  setNameForward(other): other.size() after = " << other.size() << "\n";
}

// ---------------------------------------------------------------- 5
struct Tracked {
    Tracked() = default;
    Tracked(const Tracked&) { std::cout << "copy "; }
    Tracked(Tracked&&) noexcept { std::cout << "move "; }
};

struct Order {
    Order(Tracked item, int quantity) : item_(std::move(item)), quantity_(quantity) {}
    Tracked item_;
    int quantity_;
};

template <typename T, typename... Args>
std::unique_ptr<T> make(Args&&... args) { // kao std::make_unique: savršeno prosleđivanje
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

void s05_perfectForwarding() {
    std::cout << "-- 5. perfect forwarding (variadic) --\n";
    Tracked item;
    std::cout << "  make<Order>(item, 2)            -> ";
    auto a = make<Order>(item, 2);            // lvalue: copy u parametar, move u član
    std::cout << "\n  make<Order>(std::move(item), 3) -> ";
    auto b = make<Order>(std::move(item), 3); // rvalue: move u parametar, move u član
    std::cout << "\n  <- make adds nothing: same as a direct call to new Order(...)\n";
    (void)a;
    (void)b;
}

// ---------------------------------------------------------------- 6
std::string_view firstWord(std::string_view text) { // view na tuđe podatke: pozivalac ih drži
    return text.substr(0, text.find(' '));
}

void s06_stringView() {
    std::cout << "-- 6. string_view: does not own the data --\n";
    std::string sentence = "good day everyone";
    std::string_view word = firstWord(sentence); // sentence živi duže od word: bezbedno
    std::string_view literal = "string literal";  // literal živi ceo program: bezbedno
    std::cout << "  firstWord(sentence)=\"" << word << "\", literal=\"" << literal << "\"\n";
    std::cout << "  <- a string_view to a temporary std::string dangles (ub/u01)\n";
}

// ---------------------------------------------------------------- 7
void s07_iteratorInvalidation() {
    std::cout << "-- 7. iterator invalidation --\n";
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    for (auto it = v.begin(); it != v.end();) {
        if (*it % 2 == 0) {
            it = v.erase(it); // erase vraća sledeći važeći iterator (bez ovoga: ub/u04)
        } else {
            ++it;
        }
    }
    std::vector<int> w{1, 2, 3, 4, 5, 6};
    w.erase(std::remove_if(w.begin(), w.end(), [](int x) { return x % 2 == 0; }), w.end()); // erase-remove
    std::cout << "  erase in a loop: " << v.size() << " elements; erase-remove: " << w.size()
              << " (C++20: std::erase_if(w, pred))\n";

    std::map<int, std::string> m{{1, "one"}};
    const std::string* stable = &m[1];
    for (int i = 2; i < 100; ++i) m[i] = "x"; // map: čvorovi se ne pomeraju
    std::cout << "  std::map after 98 insertions: same element address: " << (stable == &m[1] ? "yes" : "no")
              << "; std::vector after reallocation: no (lesson 04, ub/u06)\n";
}

// ---------------------------------------------------------------- 8
std::function<int()> makeCounterByValue() {
    int start = 10;
    return [start]() mutable { return ++start; }; // kopija: lambda nosi svoj start
}

void s08_lambdaCaptures() {
    std::cout << "-- 8. lambda capture and lifetime (EMC Item 31, 32) --\n";
    auto counter = makeCounterByValue();
    int first = counter();
    int second = counter();
    auto owned = std::make_unique<std::string>("owner");
    auto consumer = [p = std::move(owned)] { return p->size(); }; // C++14 init capture: move u lambdu
    std::cout << "  capture by value: " << first << " " << second << "; init capture move: size=" << consumer()
              << ", owned after: " << (owned ? "full" : "empty") << "\n";
    std::cout << "  <- [&] to a local that dies before the lambda: dangles (ub/u03)\n";
}

int main() {
    s01_universalReferences();
    s02_referenceCollapsing();
    s03_forward();
    s04_moveVsForward();
    s05_perfectForwarding();
    s06_stringView();
    s07_iteratorInvalidation();
    s08_lambdaCaptures();
}
