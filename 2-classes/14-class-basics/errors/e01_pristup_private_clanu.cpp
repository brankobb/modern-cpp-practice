// STD: c++17
// EXPECT-GCC: 'int Account::balance_' is private within this context
// EXPECT-CLANG: 'balance_' is a private member of 'Account'
// POGREŠNO: direktan pristup private članu izvan klase.
// Zašto: private deo vide samo funkcije članovi (i prijatelji) klase. Tako
//   klasa garantuje invarijantu (balance_ >= 0): jedini put do balance_ je
//   kroz deposit/withdraw, koji proveravaju vrednost (EC++ Item 22).
//   Provera je samo pri kompajliranju; u memoriji private ne postoji.
// Ispravno: a.deposit(100); ili dodaj javnu funkciju koja čuva invarijantu.
class Account {
public:
    void deposit(int amount) {
        if (amount > 0) balance_ += amount;
    }
    int balance() const { return balance_; }

private:
    int balance_ = 0;
};

int main() {
    Account a;
    a.balance_ = -100;
}
