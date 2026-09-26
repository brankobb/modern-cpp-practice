// STD: c++17
// EXPECT-GCC: request for member 'id' is ambiguous
// EXPECT-CLANG: non-static member 'id' found in multiple base-class subobjects of type 'Device'
// POGREŠNO: dijamant bez virtual nasleđivanja, pa pristup članu zajedničke baze.
// Zašto: Printer i Scanner svaki ima SVOJ Device, pa Copier sadrži dva
//   Device podobjekta i dva id-a. c.id ne kaže koji.
// Ispravno: c.Printer::id (ako su zaista dva uređaja), ili virtual
//   nasleđivanje (struct Printer : virtual Device) da postoji jedan Device
//   (main.cpp, sekcija 10). Još bolje: izbegni dijamant sa podacima.
struct Device {
    int id = 0;
};
struct Printer : Device {};
struct Scanner : Device {};
struct Copier : Printer, Scanner {};

int main() {
    Copier c;
    return c.id;
}
