// STD: c++17
// EXPECT-GCC: no matching function for call to 'call(<unresolved overloaded function type>, int)'
// EXPECT-CLANG: no matching function for call to 'call'
// POGREŠNO: ime preopterećene funkcije prosleđeno šablonu.
// Zašto: "process" označava DVE funkcije. Direktan poziv process(1) bira
//   po argumentu, ali šablon call(F&& f, ...) mora da dedukuje F iz samog
//   imena, a to ime nema jedan tip (EMC Item 30, lekcija 09 errors/e08).
// Ispravno: call(static_cast<int (*)(int)>(process), 1), ili lambda
//   call([](auto x) { return process(x); }, 1).
#include <utility>

int process(int x) { return x; }
int process(double x) { return static_cast<int>(x); }

template <typename F, typename A>
int call(F&& f, A&& a) { return std::forward<F>(f)(std::forward<A>(a)); }

int main() {
    return call(process, 1);
}
