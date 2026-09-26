// STD: c++17
// EXPECT-GCC: casts away qualifiers
// EXPECT-CLANG: casts away qualifiers
// POGREŠNO: reinterpret_cast sa const int* na unsigned char*.
// Zašto: const sme da skine SAMO const_cast (lekcija 09). Svaki imenovani
//   cast radi jednu vrstu posla, pa se u kodu vidi koja je namera.
//   C-cast bi ovde uradio oba koraka odjednom, bez upozorenja.
// Ispravno: reinterpret_cast<const unsigned char*>(&x) -- čitanje bajtova
//   ne traži uklanjanje const.
int main() {
    const int x = 5;
    unsigned char* p = reinterpret_cast<unsigned char*>(&x);
    return *p;
}
