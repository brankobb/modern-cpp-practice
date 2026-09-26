// EXPECT-GCC: binary expression in operand of fold-expression
// EXPECT-CLANG: expression not permitted as operand of fold expression
// POGREŠNO: operand fold-a mora biti "prost" izraz (cast-expression).
// args * 2 + ... bi bilo dvosmisleno (šta se fold-uje, * ili +?), pa
// je zabranjeno.
// Ispravno: dodatne zagrade oko operanda -- return ((args * 2) + ...);
template <typename... T>
int zbirDuplih(T... args) {
    return (args * 2 + ...);
}
int main() { return zbirDuplih(1, 2); }
