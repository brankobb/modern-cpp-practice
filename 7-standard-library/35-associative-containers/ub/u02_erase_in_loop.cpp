// FLAGS: -D_GLIBCXX_DEBUG
// EXPECT-UB: attempt to increment a singular iterator
// UB: m.erase(it) u petlji, pa ++it na obrisanom iteratoru. ASan ovo sam
// NE vidi: ++it za mapu se izvršava unutar libstdc++ (neinstrumentisan
// kod, lekcija 07, sekcija 7), pa program "radi". Debug režim libstdc++
// (-D_GLIBCXX_DEBUG) proverava iteratore i zaustavi program.
// Ispravno:
//   for (auto it = m.begin(); it != m.end();)
//       if (it->second < 0) it = m.erase(it); else ++it;
// ili C++20: std::erase_if(m, [](const auto& p) { return p.second < 0; });
#include <iostream>
#include <map>
int main() {
    std::map<int, int> m{{1, 10}, {2, -1}, {3, 30}};
    for (auto it = m.begin(); it != m.end(); ++it)
        if (it->second < 0) m.erase(it);
    std::cout << m.size() << '\n';
}
