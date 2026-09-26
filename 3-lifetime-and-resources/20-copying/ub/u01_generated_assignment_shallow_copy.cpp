// EXPECT-UB: attempting double-free
// POGREŠNO: klasa poseduje memoriju preko char*, a copy dodela je kompajlerova.
// Zašto: b = a kopira POKAZIVAČ: b.data_ sada pokazuje na a-ovu memoriju.
//   Dve greške odjednom: b-ova stara memorija ("Bob") više nema vlasnika
//   (curenje), a na kraju main-a oba destruktora obrišu istu memoriju.
//   Klasa ima destruktor, a nema copy ctor i copy dodelu: rule of 3 prekršen.
// Ispravno: sve tri funkcije (main.cpp, sekcija 4), ili zabrani kopiranje
//   (= delete), ili std::string umesto char* (rule of 0).
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
    Name a("Ann");
    Name b("Bob");
    b = a;
    std::printf("%s\n", b.c_str());
}
