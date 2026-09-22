#include <iostream>
#include <limits>

// Pre pokretanja: za svaki blok probaj da predvidiš ispis, PA pokreni.

void signedUnsignedTrap() {
    int a = -1;
    unsigned b = 0;
    // a se konvertuje u unsigned pre poređenja -> -1 postaje UINT_MAX
    std::cout << "(a > b) = " << (a > b) << " (očekivano: iznenađenje)\n";
}

void overflowTrap() {
    int max = std::numeric_limits<int>::max();
    std::cout << "max = " << max << "\n";
    std::cout << "max + 1 = " << (max + 1) << " (signed overflow = UB, pokreni sa -fsanitize=undefined!)\n";

    unsigned int umax = std::numeric_limits<unsigned int>::max();
    std::cout << "umax + 1 = " << (umax + 1) << " (unsigned wraparound je DEFINISAN, očekuj 0)\n";
}

void cinFailStateTrap() {
    std::cout << "Unesi broj (probaj i slovo umesto broja): ";
    int x;
    if (!(std::cin >> x)) {
        std::cout << "cin je u fail state-u. Bez clear()+ignore() sledeća čitanja ce se ignorisati.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cout << "Uneo si: " << x << "\n";
}

int add(int a, int b) { return a + b; } // parametri po vrednosti -- kopija

int main() {
    signedUnsignedTrap();
    overflowTrap();
    cinFailStateTrap();
    std::cout << add(2, 3) << "\n";
}
