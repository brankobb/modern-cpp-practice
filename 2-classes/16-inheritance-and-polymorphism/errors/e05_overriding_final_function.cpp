// STD: c++17
// EXPECT-GCC: overriding final function
// EXPECT-CLANG: declaration of 'id' overrides a 'final' function
// POGREŠNO: override funkcije koja je u baznoj klasi označena sa final.
// Zašto: Mid::id() je final: nijedna klasa ispod Mid ne sme da je promeni.
// Ispravno: ne nadjačavaj id() u Leaf, ili ukloni final u Mid.
class Base {
public:
    virtual ~Base() = default;
    virtual int id() const { return 0; }
};

class Mid : public Base {
public:
    int id() const final { return 1; }
};

class Leaf : public Mid {
public:
    int id() const override { return 2; }
};

int main() {}
