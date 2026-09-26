// KIND: usage
//
// Zadatak 1 -- overloading, podrazumevani argumenti, callback (sekcije 1, 2, 6, 7)
//   ./build.sh 1-language-basics/11-functions-advanced/exercises/ex1_overload_callback.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_overload_callback.cpp
//
// Korak 1: četiri overload-a void opisi(...) za int, double, const char*
//   i bool; svaki ispiše tip i vrednost ("int 42"). PRE pokretanja
//   predvidi koji overload bira opisi('a') i opisi(1.5f)
//   (sekcija 2: promocija char -> int i float -> double je bolja od
//   konverzije).
// Korak 2: std::string formatiraj(double v, int decimale = 2, char sep = '.')
//   -- ispis sa zadatim brojem decimala (std::ostringstream, std::fixed,
//   std::setprecision), pa zameni '.' sa sep. Podrazumevane vrednosti su
//   samo u DEKLARACIJI, i samo sa desne strane.
// Korak 3: struct Dugme sa članom void (*naKlik)(int) = nullptr; i metodom
//   void klik(int x) koja pozove callback ako postoji, a inače ispiše
//   "dugme bez handlera". Handler je obična funkcija
//   void prijavi(int x) koja ispiše "klik: x". Lambda BEZ hvatanja se
//   takođe konvertuje u pokazivač na funkciju -- probaj i nju.

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- predvidi, pa otkomentariši:
    // opisi(42);
    // opisi('a');           // predviđanje: ?
    // opisi(1.5f);          // predviđanje: ?
    // opisi("x");
    // opisi(true);

    // Korak 2 -- otkomentariši:
    // std::cout << formatiraj(3.14159) << ' ' << formatiraj(3.14159, 3) << ' '
    //           << formatiraj(3.14159, 2, ',') << '\n';

    // Korak 3 -- otkomentariši:
    // Dugme d;
    // d.klik(1);
    // d.naKlik = prijavi;
    // d.klik(5);
    // d.naKlik = [](int x) { std::cout << "lambda: " << x * 10 << '\n'; };
    // d.klik(5);
}

/* EXPECTED OUTPUT
int 42
int 97
double 1.5
const char* x
bool 1
3.14 3.142 3,14
dugme bez handlera
klik: 5
lambda: 50
*/
