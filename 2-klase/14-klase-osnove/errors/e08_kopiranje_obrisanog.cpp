// STD: c++17
// EXPECT-GCC: use of deleted function 'Handle::Handle(const Handle&)'
// EXPECT-CLANG: call to deleted constructor of 'Handle'
// POGREŠNO: kopiranje objekta čiji je copy konstruktor "= delete".
// Zašto: Handle predstavlja jedinstven resurs (fajl, konekcija), pa je
//   kopiranje namerno zabranjeno (EC++ Item 6, EMC Item 11). Prosleđivanje
//   po vrednosti je kopija. Greška je pri kompajliranju, ne tek pri
//   pokretanju.
// Ispravno: prosledi po referenci (void use(const Handle& h)), ili prebaci
//   vlasništvo move-om (lekcija 22).
class Handle {
public:
    Handle() = default;
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
};

void use(Handle h) { (void)h; }

int main() {
    Handle h;
    use(h);
}
