// KIND: why
// DEMO-OUT: NAIVE sent: \[temp=21;humidity=40\]
//
// Zadatak 3 -- zašto data() od string_view-a nije C string (sekcija 3; errors/e03)
// Rešenje: exercises/solutions/ex3_not_a_c_string.cpp
//
// Konfiguracija se seče na delove (string_view), a svaki deo se šalje
// C funkciji sendToPort(const char*) -- kao drajver serijskog porta.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 7-standard-library/38-string-view-and-filesystem/exercises/ex3_not_a_c_string.cpp -DNAIVE
//   Prvi deo je "temp=21", a poslato je "temp=21;vlaga=40". data() je
//   samo pokazivač na PRVI znak pogleda; C funkcija čita do '\0', a '\0'
//   postoji tek na kraju celog originala. Nije UB samo zato što je
//   original ceo string sa '\0'; da je pogled na bafer bez '\0',
//   čitalo bi se van njega.
// Korak 2: u #else grani napiši send(std::string_view) tako da C
//   funkcija dobije tačno deo: kopija std::string(deo), pa .c_str().

#include <cstdio>
#include <string>
#include <string_view>

void sendToPort(const char* message) { std::printf("sent: [%s]\n", message); }   // "C API"

#ifdef NAIVE
void send(std::string_view part) { sendToPort(part.data()); }
#else
// TODO korak 2 (dok ne napišeš, šalje se "?")
void send(std::string_view) { sendToPort("?"); }
#endif

int main() {
    std::string config = "temp=21;humidity=40";
    std::string_view all = config;
    auto sep = all.find(';');
    send(all.substr(0, sep));
    send(all.substr(sep + 1));
}

/* EXPECTED OUTPUT
sent: [temp=21]
sent: [humidity=40]
*/
