// STD: c++17
// EXPECT-GCC: invalid use of member 'Counter::value_' in static member function
// EXPECT-CLANG: invalid use of member 'value_' in static member function
// POGREŠNO: static funkcija člana čita ne-static član.
// Zašto: static funkcija pripada KLASI, a ne objektu, pa nema this. value_
//   postoji posebno u svakom objektu, a ovde nema objekta čiji value_ bi
//   se čitao. Counter::twice() se poziva i kad nijedan objekat ne postoji.
// Ispravno: neka funkcija ne bude static, ili neka primi objekat kao
//   parametar (static int twice(const Counter& c)), ili neka koristi samo
//   static članove.
class Counter {
public:
    static int twice() { return 2 * value_; }

private:
    int value_ = 0;
};

int main() {
    return Counter::twice();
}
