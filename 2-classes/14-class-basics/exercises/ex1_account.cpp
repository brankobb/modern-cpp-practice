// KIND: usage
//
// Zadatak 1 -- klasa sa invarijantom, delegiranje, this i static (sekcije 1, 2, 5, 6, 9)
//   ./build.sh 2-classes/14-class-basics/exercises/ex1_account.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_account.cpp
//
// class Account: privatni std::string owner_ i long balance_ (u parama).
// Invarijanta: balance_ >= 0 u svakom trenutku.
// Korak 1: konstruktor Account(std::string owner, long initial) baca
//   std::invalid_argument ako je initial < 0 (objekat sa pokvarenom
//   invarijantom tada ni ne nastaje). Drugi konstruktor,
//   explicit Account(std::string owner), DELEGIRA prvom sa initial = 0.
// Korak 2: Account& deposit(long amount) vraća *this, pa se pozivi mogu
//   nizati: a.deposit(100).deposit(50). bool withdraw(long amount) ne
//   dozvoli minus (vrati false). long balance() const,
//   const std::string& owner() const.
// Korak 3: static int aliveCount() -- koliko Account objekata trenutno
//   postoji. Brojač je static član (jedan za celu klasu). Uvećaj ga u SVAKOM
//   konstruktoru koji zaista pravi objekat (pazi na delegiranje: ne broji
//   dvaput!), umanji u destruktoru. Kopiju zabrani (= delete) -- račun se
//   ne kopira.

#include <iostream>
#include <stdexcept>
#include <string>

class Account {
public:
    // TODO korak 1, 2, 3
};

int main() {
    // Korak 1 i 2 -- otkomentariši:
    // Account a("Ann", 1000);
    // a.deposit(100).deposit(50);
    // bool ok = a.withdraw(2000);
    // std::cout << a.owner() << ": " << a.balance() << ", withdraw 2000: " << ok << '\n';
    // try {
    //     Account bad("Bad", -5);
    // } catch (const std::invalid_argument& e) {
    //     std::cout << "rejected: " << e.what() << '\n';
    // }

    // Korak 3 -- otkomentariši:
    // std::cout << "alive: " << Account::aliveCount() << '\n';
    // {
    //     Account b("Bob");
    //     std::cout << "alive in block: " << Account::aliveCount() << ", Bob: " << b.balance() << '\n';
    // }
    // std::cout << "alive after block: " << Account::aliveCount() << '\n';
}

/* EXPECTED OUTPUT
Ann: 1150, withdraw 2000: 0
rejected: initial balance < 0
alive: 1
alive in block: 2, Bob: 0
alive after block: 1
*/
