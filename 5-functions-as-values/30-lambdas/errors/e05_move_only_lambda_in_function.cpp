// EXPECT-GCC: std::function target must be copy-constructible
// EXPECT-CLANG: std::function target must be copy-constructible
// POGREŠNO: std::function mora da može da se kopira, pa traži da se i
// ono što drži može kopirati. Lambda koja drži unique_ptr to ne može.
// Greška dolazi iz <functional> (static_assert u libstdc++).
// Ispravno: čuvaj lambdu kao auto, ili je prosledi šablonu (sekcija 8);
// ako baš treba std::function, deljeno vlasništvo (shared_ptr) umesto
// unique_ptr. (C++23 ima std::move_only_function.)
#include <functional>
#include <memory>
#include <utility>
int main() {
    auto p = std::make_unique<int>(7);
    std::function<int()> f = [q = std::move(p)] { return *q; };
    return f();
}
