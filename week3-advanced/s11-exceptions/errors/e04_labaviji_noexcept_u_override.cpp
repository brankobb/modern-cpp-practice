// EXPECT-GCC: looser exception specification on overriding virtual function
// EXPECT-CLANG: exception specification of overriding function is more lax than base version
// POGREŠNO: bazna funkcija obećava noexcept, a pozivalac preko Senzor& se
// na to oslanja. Izvedena klasa ne sme da prekrši obećanje baze.
// Ispravno: double ocitaj() const noexcept override -- i u telu ne bacati
// (grešku vrati kao vrednost, ili je obradi unutra).
struct Senzor {
    virtual ~Senzor() = default;
    virtual double ocitaj() const noexcept { return 0.0; }
};
struct TermoSenzor : Senzor {
    double ocitaj() const override { return 21.5; }
};
int main() {
    TermoSenzor t;
    return static_cast<int>(t.ocitaj());
}
