// STD: c++17
// EXPECT-GCC: in read-only object
// EXPECT-CLANG: within const member function
// POGREŠNO: const member funkcija obećava da ne menja objekat.
// Ispravno: skini const sa funkcije, ili -- ako je to samo keš -- mutable član.
struct Counter {
    int value = 0;
    void reset() const { value = 0; }
};
int main() {
    Counter c;
    c.reset();
}
