// VRSTA: zašto
// DEMO-OUT: NAIVNO ime: pogrešan tip \(const char\*\)
//
// Zadatak 3 -- zašto any_cast<std::string> ne uspe na "temp" (sekcija 7)
// Rešenje: exercises/solutions/z3_any_literal.cpp
//
// Svojstva uređaja su u std::map<std::string, std::any>.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week3-advanced/s22-optional-variant-any/exercises/z3_any_literal.cpp -DNAIVNO
//   Čitanje imena baci bad_any_cast. svojstva["ime"] = "temp" upiše
//   const char* (literal se "raspadne" u pokazivač), a any_cast traži
//   TAČAN tip -- bez konverzija, iako se const char* inače pretvara u
//   std::string. Ista zamka kao CTAD sa literalom (s21, zadatak z3).
//   (U ovom primeru const char* pokazuje na literal, pa bar ne visi; da
//   je upisan c_str() lokalnog stringa, pokazivao bi u oslobođenu memoriju.)
// Korak 2: u #else grani napiši postaviIme() tako da u any ode
//   std::string (std::string("temp") ili "temp"s).

#include <any>
#include <iostream>
#include <map>
#include <string>

using Svojstva = std::map<std::string, std::any>;

#ifdef NAIVNO
void postaviIme(Svojstva& s) { s["ime"] = "temp"; }
#else
// TODO korak 2 (dok ne napišeš, ime se ne postavlja)
void postaviIme(Svojstva&) {}
#endif

int main() {
    Svojstva s;
    s["id"] = 7;
    postaviIme(s);
    std::cout << "id: " << std::any_cast<int>(s["id"]) << '\n';
    try {
        std::cout << "ime: " << std::any_cast<std::string>(s.at("ime")) << '\n';
    } catch (const std::bad_any_cast&) {
        bool pokazivac = s.at("ime").type() == typeid(const char*);
        std::cout << "pogrešan tip" << (pokazivac ? " (const char*)" : "") << '\n';
    } catch (const std::out_of_range&) {
        std::cout << "nije postavljeno\n";
    }
}

/* OČEKIVANI IZLAZ
id: 7
ime: temp
*/
