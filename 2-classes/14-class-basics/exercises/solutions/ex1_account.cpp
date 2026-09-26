// Rešenje zadatka ex1_account.

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

class Account {
public:
    // Korak 1: provera PRE nego što objekat postoji. Ako konstruktor baci,
    // destruktor se ne poziva, pa ni brojač ne sme biti uvećan -- zato
    // ++alive_ ide na kraj tela.
    Account(std::string owner, long initial) : owner_(std::move(owner)), balance_(initial) {
        if (initial < 0) throw std::invalid_argument("initial balance < 0");
        ++alive_;
    }
    // Delegirajući: ceo posao radi ciljni konstruktor (i on broji), pa se
    // ovde ne broji ponovo.
    explicit Account(std::string owner) : Account(std::move(owner), 0) {}

    ~Account() { --alive_; }

    Account(const Account&) = delete;
    Account& operator=(const Account&) = delete;

    // Korak 2: vraćanje *this po referenci omogućava nizanje poziva.
    Account& deposit(long amount) {
        balance_ += amount;
        return *this;
    }
    bool withdraw(long amount) {
        if (amount > balance_) return false;   // invarijanta: nikad ispod 0
        balance_ -= amount;
        return true;
    }
    long balance() const { return balance_; }
    const std::string& owner() const { return owner_; }

    // Korak 3: static funkcija nema this -- vidi samo static članove.
    static int aliveCount() { return alive_; }

private:
    std::string owner_;
    long balance_;
    static inline int alive_ = 0;   // C++17: inline static, bez definicije van klase
};

int main() {
    Account a("Ann", 1000);
    a.deposit(100).deposit(50);
    bool ok = a.withdraw(2000);
    std::cout << a.owner() << ": " << a.balance() << ", withdraw 2000: " << ok << '\n';
    try {
        Account bad("Bad", -5);
    } catch (const std::invalid_argument& e) {
        std::cout << "rejected: " << e.what() << '\n';
    }

    std::cout << "alive: " << Account::aliveCount() << '\n';
    {
        Account b("Bob");
        std::cout << "alive in block: " << Account::aliveCount() << ", Bob: " << b.balance() << '\n';
    }
    std::cout << "alive after block: " << Account::aliveCount() << '\n';
}
