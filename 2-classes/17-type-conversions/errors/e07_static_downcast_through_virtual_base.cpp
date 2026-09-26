// STD: c++17
// EXPECT-GCC: because the base is virtual
// EXPECT-CLANG: via virtual base 'Device'
// POGREŠNO: static_cast naniže kroz virtual baznu klasu.
// Zašto: kod virtual nasleđivanja (lekcija 16, sekcija 10) položaj Device
//   dela unutar objekta zavisi od najizvedenije klase, pa se ne zna pri
//   kompajliranju. static_cast računa pomak pri kompajliranju, i ovde to ne
//   može.
// Ispravno: dynamic_cast<Printer&>(d), koji pomak čita pri izvršavanju.
struct Device {
    virtual ~Device() = default;
};
struct Printer : virtual Device {
    int pages = 0;
};

int main() {
    Printer p;
    Device& d = p;
    Printer& back = static_cast<Printer&>(d);
    return back.pages;
}
