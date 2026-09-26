// KIND: why
// DEMO-OUT: NAIVE name: wrong type \(const char\*\)
//
// Zadatak 3 -- zašto any_cast<std::string> ne uspe na "temp" (sekcija 7)
// Rešenje: exercises/solutions/ex3_any_literal.cpp
//
// Svojstva uređaja su u std::map<std::string, std::any>.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 7-standard-library/37-optional-variant-any/exercises/ex3_any_literal.cpp -DNAIVE
//   Čitanje imena baci bad_any_cast. svojstva["ime"] = "temp" upiše
//   const char* (literal se "raspadne" u pokazivač), a any_cast traži
//   TAČAN tip -- bez konverzija, iako se const char* inače pretvara u
//   std::string. Ista zamka kao CTAD sa literalom (lekcija 29, zadatak ex3).
//   (U ovom primeru const char* pokazuje na literal, pa bar ne visi; da
//   je upisan c_str() lokalnog stringa, pokazivao bi u oslobođenu memoriju.)
// Korak 2: u #else grani napiši setName() tako da u any ode
//   std::string (std::string("temp") ili "temp"s).

#include <any>
#include <iostream>
#include <map>
#include <string>

using Properties = std::map<std::string, std::any>;

#ifdef NAIVE
void setName(Properties& s) { s["name"] = "temp"; }
#else
// TODO korak 2 (dok ne napišeš, ime se ne postavlja)
void setName(Properties&) {}
#endif

int main() {
    Properties s;
    s["id"] = 7;
    setName(s);
    std::cout << "id: " << std::any_cast<int>(s["id"]) << '\n';
    try {
        std::cout << "name: " << std::any_cast<std::string>(s.at("name")) << '\n';
    } catch (const std::bad_any_cast&) {
        bool isPointer = s.at("name").type() == typeid(const char*);
        std::cout << "wrong type" << (isPointer ? " (const char*)" : "") << '\n';
    } catch (const std::out_of_range&) {
        std::cout << "not set\n";
    }
}

/* EXPECTED OUTPUT
id: 7
name: temp
*/
