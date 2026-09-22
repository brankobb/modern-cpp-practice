// EXPECT-UB: caused by a WRITE memory access
// UB: string literal je niz const char. U C-u se istorijski dodeljivao u
// char*, ali upis u njega je UB -- literal je u memoriji samo za čitanje.
// Ispravno: char buf[] = "hello"; buf[0] = 'H';  (kopija na steku)
//       ili: std::string s = "hello"; s[0] = 'H';
#include <iostream>
int main() {
    char* s = const_cast<char*>("hello");
    s[0] = 'H';
    std::cout << s << "\n";
}
