// STD: c++17
// EXPECT-GCC: ISO C++ prohibits overloading 'operator ?:'
// EXPECT-CLANG: expected a type
// POGREŠNO: pokušaj da se preoptereti ternarni operator ?:.
// Zašto: neki operatori se ne mogu preopteretiti: ?: . .* :: sizeof typeid
//   alignof i cast-ovi ([over.oper]). Uglavnom su to operatori čiji desni
//   operand nije vrednost nego ime (a.b, A::b), ili oni koji moraju da
//   ostanu ugrađeni da bi jezik uopšte radio. Ne mogu se ni izmisliti novi
//   operatori (**), ni promeniti prioritet ili broj operanada (e04).
// Ispravno: obična funkcija sa opisnim imenom (choose(cond, a, b)).
struct Vec {
    double x;
    Vec operator?:(const Vec& a, const Vec& b) const;
};

int main() {}
