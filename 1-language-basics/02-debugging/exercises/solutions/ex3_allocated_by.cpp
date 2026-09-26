// Rešenje zadatka ex3_allocated_by.
//
// Korak 1 (iz izveštaja):
// a) heap-buffer-overflow, WRITE of size 10 -- strcpy upisuje 9 znakova i
//    '\0';
// b) #0 je strcpy (ASan ga presreće), #1 duplicate() -- red sa strcpy;
// c) "0 bytes after 9-byte region": blok ima 9 bajtova, a treba 10;
// d) "allocated by" pokazuje red sa new char[std::strlen(s)] -- tu je bag:
//    strlen ne računa završni '\0'.

#include <cstring>
#include <iostream>
#include <string>

// Ako alociraš strlen(s) bajtova (nije dobro): nema mesta za '\0', pa
// strcpy upiše jedan bajt iza bloka.
// Treba ovako: strlen(s) + 1.
char* duplicate(const char* s) {
    char* p = new char[std::strlen(s) + 1];
    std::strcpy(p, s);
    return p;
}

// Možeš i ovako: std::string sam vodi računa o veličini i oslobađanju.
std::string duplicateString(const char* s) { return std::string(s); }

int main() {
    char* d = duplicate("sensor-01");
    std::cout << d << '\n';
    delete[] d;
    std::cout << duplicateString("sensor-02") << '\n';
}
