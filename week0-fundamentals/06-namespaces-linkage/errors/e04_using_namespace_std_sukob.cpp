// STD: c++17
// EXPECT-GCC: reference to 'count' is ambiguous
// EXPECT-CLANG: reference to 'count' is ambiguous
// POGREŠNO: "using namespace std;" na nivou fajla + sopstvena globalna
//   promenljiva sa imenom koje već postoji u std (std::count iz <algorithm>).
// Zašto: direktiva uvodi SVA imena iz std, a std ih ima na stotine (count,
//   distance, size, data, min, max...). Tvoje ::count i std::count su sada
//   oba vidljiva i nijedno nema prednost. U header-u je gore: problem dobija
//   svako ko uključi header (Core Guidelines SF.7).
// Ispravno: bez "using namespace std;", ili using-deklaracije za tačno ono što
//   koristiš (using std::cout;), ili ::count za globalnu promenljivu.
#include <algorithm>
using namespace std;

int count = 0;

int main() {
    count++;
}
