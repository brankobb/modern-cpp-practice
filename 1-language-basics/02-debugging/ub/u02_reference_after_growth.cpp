// EXPECT-UB: heap-use-after-free
// BAG za vežbu (notes.md, sekcija 5): referenca na element vektora, pa
// push_back koji realocira. ASan izveštaj ima TRI steka:
//   READ ... #0 main        -- gde je čitano (linija sa std::cout)
//   freed by ... _M_deallocate / push_back  -- gde je memorija oslobođena
//   previously allocated by ... _M_allocate  -- gde je bila alocirana
// Uzrok je u drugom steku (push_back), a ne tamo gde je program pao.
// Ispravno: indeks umesto reference, ili referenca tek posle push_back-a
// (lekcija 04, sekcija 12).
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v{1, 2, 3};
    int& first = v[0];
    v.push_back(4);
    std::cout << first << '\n';
}
