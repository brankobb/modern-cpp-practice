#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// Vežba A: napiši template funkciju
//   template <typename T> void wrapper(T&& arg) { inner(std::forward<T>(arg)); }
// sa dva overload-a za inner (const std::string&, std::string&&) koji
// loguju koji je pozvan. Pozovi wrapper i sa lvalue i sa rvalue string-om.
//
// Vežba B: namerno napravi dangling string_view (uzmi ga iz privremenog
// std::string koji se odmah uništi) i pokreni pod ASan-om da vidiš
// use-after-free/stack-use-after-scope.
//
// Vežba C: invalidacija iteratora -- napravi vector, uzmi iterator, pa
// push_back dovoljno puta da izazove realokaciju, pa koristi stari iterator.

void inner(const std::string&) { std::cout << "inner(const&)\n"; }
void inner(std::string&&) { std::cout << "inner(&&)\n"; }

template <typename T>
void wrapper(T&& arg) {
    inner(std::forward<T>(arg));
}

int main() {
    std::string s = "hi";
    wrapper(s);            // treba: inner(const&)
    wrapper(std::string("temp")); // treba: inner(&&)

    // TODO: dangling string_view primer (proveri pod ASan-om)
    // TODO: invalidacija iteratora primer
}
