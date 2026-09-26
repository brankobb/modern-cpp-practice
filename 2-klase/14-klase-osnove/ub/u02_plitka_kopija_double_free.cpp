// EXPECT-UB: attempting double-free
// POGREŠNO: klasa poseduje memoriju preko sirovog pokazivača, a copy
//   konstruktor je kompajlerov.
// Zašto: kompajlerov copy konstruktor kopira član po član, pa b.data_ dobije
//   ISTU adresu kao a.data_ (plitka kopija). Na kraju b-ovog scope-a
//   destruktor obriše tu memoriju, a na kraju main-a a-ov destruktor je
//   obriše ponovo.
// Ispravno: duboka kopija u sopstvenom copy konstruktoru (main.cpp,
//   sekcija 8), ili zabrani kopiranje (= delete), ili std::string umesto
//   char* pa kopija radi sama. Celo pravilo (rule of 3) je u lekciji 20.
#include <cstdio>
#include <cstring>

class Name {
public:
    explicit Name(const char* s) : data_(new char[std::strlen(s) + 1]) { std::strcpy(data_, s); }
    ~Name() { delete[] data_; }
    const char* c_str() const { return data_; }

private:
    char* data_;
};

int main() {
    Name a("Ana");
    {
        Name b = a;
        std::printf("%s\n", b.c_str());
    }
}
