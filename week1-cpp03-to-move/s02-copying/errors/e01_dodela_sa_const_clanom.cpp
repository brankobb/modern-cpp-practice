// STD: c++17
// EXPECT-GCC: use of deleted function 'Order& Order::operator=(const Order&)'
// EXPECT-CLANG: copy assignment operator is implicitly deleted
// POGREŠNO: dodela objekta čiji je član const.
// Zašto: kompajlerova dodela dodeljuje član po član, a const int id ne
//   može da se dodeli. Zato je operator= OBRISAN ([class.copy.assign]).
//   Kopija (Order c = a;) i dalje radi: tu se id INICIJALIZUJE.
// Ispravno: ako objekat treba da se dodeljuje, član nije const (čuvaj
//   invarijantu kroz private + getter); ako ne treba, ovo je i željeno.
struct Order {
    const int id;
    int quantity;
};

int main() {
    Order a{1, 5};
    Order b{2, 7};
    a = b;
    return a.quantity;
}
