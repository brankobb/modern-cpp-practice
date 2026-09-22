#include <iostream>
#include <limits>

// Pre pokretanja: za svaki blok probaj da predvidiš ispis, PA pokreni.

void signedUnsignedTrap() {
    std::cout << "-- signedUnsignedTrap --\n";
    int a = -1;
    unsigned b = 0;
    // Ako mešaš signed i unsigned u poređenju (NIJE DOBRO) jer kompajler
    // tiho konvertuje signed operand u unsigned pre poređenja, pa -1
    // postane ogroman broj (UINT_MAX) -- bag koji se ne vidi na prvi
    // pogled u kodu.
    // Treba da oba operanda budu ISTOG signedness-a (npr. uporedi
    // static_cast<int>(b) sa a) da poređenje radi ono što očekuješ.
    // Možeš i eksplicitno cast-ovati u unsigned ako si SIGURAN da je
    // vrednost nenegativna (static_cast<unsigned>(a) > b) -- ali tad je
    // to namerna odluka, ne slučajna implicitna konverzija.
    std::cout << "(a > b) = " << (a > b) << " (očekivano: iznenađenje)\n";
}

void overflowTrap() {
    std::cout << "-- overflowTrap --\n";
    int max = std::numeric_limits<int>::max();
    std::cout << "max = " << max << "\n";
    // Ako uradiš max + 1 na signed int-u bez provere (NIJE DOBRO) jer je
    // signed integer overflow UNDEFINED BEHAVIOR po standardu -- kompajler
    // sme da PRETPOSTAVI da se to nikad ne dešava i optimizuje kod na
    // iznenađujuće načine (ne samo da "wrap-uje" kao što bi neko očekivao).
    // Treba da PRE operacije proveriš granice (npr. if (max >
    // std::numeric_limits<int>::max() - 1) ...) ili koristiš veći tip
    // (long long) ako očekuješ da vrednosti mogu da priđu granici.
    // Možeš i koristiti unsigned tip ako ti DEFINISAN wraparound odgovara
    // (npr. heš funkcije namerno računaju "mod 2^32") -- ali to je
    // svesna odluka o semantici, ne zamena za signed po defaultu.
    std::cout << "max + 1 = " << (max + 1) << " (signed overflow = UB, pokreni sa -fsanitize=undefined!)\n";

    unsigned int umax = std::numeric_limits<unsigned int>::max();
    std::cout << "umax + 1 = " << (umax + 1) << " (unsigned wraparound je DEFINISAN, očekuj 0)\n";
}

void cinFailStateTrap() {
    std::cout << "-- cinFailStateTrap --\n";
    std::cout << "Unesi broj (probaj i slovo umesto broja): ";
    int x;
    if (!(std::cin >> x)) {
        // Ako samo ispišeš grešku i nastaviš dalje (NIJE DOBRO) jer stream
        // ostaje u fail state-u -- SVA sledeća čitanja će se tiho
        // ignorisati (x ostaje nepromenjen, program "izgleda" da visi ili
        // preskače unos bez objašnjenja).
        // Treba da pozoveš clear() (resetuje fail/bad flagove) PA ignore()
        // (izbaci ostatak lošeg unosa iz bafera) pre sledećeg čitanja.
        // Možeš i koristiti std::getline + std::stringstream umesto
        // cin >> x direktno -- robusnije za složeniji unos, ali više koda
        // za ovako jednostavan slučaj.
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
    std::cout << "-- add(2, 3) --\n";
    std::cout << add(2, 3) << "\n";
}
