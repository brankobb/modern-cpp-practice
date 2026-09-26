// Rešenje zadatka ex3_remove_does_not_erase.

#include <algorithm>
#include <iostream>
#include <vector>

// Ako samo pozoveš std::remove (nije dobro): elementi koji ostaju se
// prepišu ka početku, ali veličina se ne menja -- algoritam vidi samo
// iteratore, ne vektor.
// Treba ovako: erase-remove -- remove vrati novi kraj, erase ukloni rep.
void removeAll(std::vector<int>& v, int x) { v.erase(std::remove(v.begin(), v.end(), x), v.end()); }

// Korak 3: list::remove prevezuje čvorove (O(1) po elementu, bez
// prepisivanja), što opšti algoritam ne ume; za vector bi takva metoda
// radila isto što i erase-remove, pa je standard nije dodao (C++20 ima
// slobodnu funkciju std::erase za sve kontejnere).

int main() {
    std::vector<int> channels{1, 2, 3, 2, 5, 2};
    removeAll(channels, 2);
    std::cout << "after removal:";
    for (int k : channels) std::cout << ' ' << k;
    std::cout << " (size " << channels.size() << ")\n";
}
