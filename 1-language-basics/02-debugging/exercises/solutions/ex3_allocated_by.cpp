// Rešenje zadatka ex3_allocated_by.
//
// Korak 1 (iz izveštaja):
// a) heap-buffer-overflow, WRITE of size 10 -- strcpy upisuje 9 znakova i
//    '\0';
// b) #0 je strcpy (ASan ga presreće), #1 kopija() -- red sa strcpy;
// c) "0 bytes after 9-byte region": blok ima 9 bajtova, a treba 10;
// d) "allocated by" pokazuje red sa new char[std::strlen(s)] -- tu je bag:
//    strlen ne računa završni '\0'.

#include <cstring>
#include <iostream>
#include <string>

// Ako alociraš strlen(s) bajtova (nije dobro): nema mesta za '\0', pa
// strcpy upiše jedan bajt iza bloka.
// Treba ovako: strlen(s) + 1.
char* kopija(const char* s) {
    char* p = new char[std::strlen(s) + 1];
    std::strcpy(p, s);
    return p;
}

// Možeš i ovako: std::string sam vodi računa o veličini i oslobađanju.
std::string kopijaS(const char* s) { return std::string(s); }

int main() {
    char* k = kopija("senzor-01");
    std::cout << k << '\n';
    delete[] k;
    std::cout << kopijaS("senzor-02") << '\n';
}
