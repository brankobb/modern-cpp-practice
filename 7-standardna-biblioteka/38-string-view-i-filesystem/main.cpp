#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <new>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

// std::string_view i std::filesystem (C++17) -- ISPRAVNI slučajevi.
// Sve se kompajlira bez upozorenja i radi bez ASan/UBSan prijava (g++ 13
// i clang 18, C++17 i C++20). Brojevi sekcija prate notes.md. Broj
// alokacija je za libstdc++ (SSO bafer od 15 znakova, rast vektora 1, 2, 4).
// Filesystem deo radi u privremenom direktorijumu (temp_directory_path)
// i na kraju ga obriše; ispisuje samo putanje relativne u odnosu na njega,
// a sadržaj direktorijuma sortira -- redosled iteracije nije određen.
// POGREŠNI slučajevi:
//   errors/   -- kod koji se NE kompajlira
//   ub/       -- string_view koji nadživi string
//   runtime/  -- filesystem_error, substr van opsega, bez try
// ./check_cases.sh 7-standardna-biblioteka/38-string-view-i-filesystem  proverava sve.

namespace fs = std::filesystem;
using namespace std::string_view_literals;

static int alokacija = 0;   // brojač za sekciju 2
void* operator new(std::size_t n) {
    ++alokacija;
    if (void* p = std::malloc(n)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

// ---------------------------------------------------------------- 1
void sekcija1() {
    std::cout << "\n== 1. string_view: pogled na tuđe znakove\n";
    std::string s = "temperatura";
    const char niz[] = {'v', 'l', 'a', 'g', 'a'};       // bez '\0'
    std::string_view a = "pritisak";                    // literal
    std::string_view b = s;                             // std::string
    std::string_view c(niz, sizeof(niz));               // pokazivač + dužina
    auto d = "napon"sv;                                 // sv literal
    std::cout << a << ' ' << b << ' ' << c << ' ' << d << '\n';
    std::cout << "sizeof(string_view) = " << sizeof(std::string_view) << " (pokazivač + dužina)\n";

    std::string_view t = b.substr(0, 5);                // "tempe" -- bez kopiranja
    std::cout << "substr(0, 5): " << t << ", ista memorija kao s: " << (t.data() == s.data()) << '\n';
    std::string_view red = "  [temp] 21.5  ";
    red.remove_prefix(red.find_first_not_of(' '));      // skida sa početka POGLEDA
    red.remove_suffix(red.size() - red.find_last_not_of(' ') - 1);
    std::cout << "posle remove_prefix/suffix: \"" << red << "\"\n";
    std::cout << "find(']') = " << red.find(']') << ", poređenje sa \"[temp] 21.5\": "
              << (red == "[temp] 21.5") << '\n';
    std::string kopija(red);                            // u std::string samo EKSPLICITNO (errors/e01)
    std::cout << "std::string(red).size() = " << kopija.size() << '\n';
}

// ---------------------------------------------------------------- 2
std::size_t duzinaStr(const std::string& s) { return s.size(); }
std::size_t duzinaSv(std::string_view s) { return s.size(); }

std::vector<std::string_view> podeli(std::string_view tekst, char sep) {
    std::vector<std::string_view> delovi;
    while (true) {
        auto poz = tekst.find(sep);
        delovi.push_back(tekst.substr(0, poz));
        if (poz == std::string_view::npos) break;
        tekst.remove_prefix(poz + 1);
    }
    return delovi;
}

void sekcija2() {
    std::cout << "\n== 2. string_view kao parametar: bez kopija\n";
    const char* dug = "senzor temperature u hali broj 3";   // duže od SSO bafera (15 znakova)
    alokacija = 0;
    duzinaStr(dug);                                     // pravi privremeni std::string
    int zaStr = alokacija;
    alokacija = 0;
    duzinaSv(dug);
    std::cout << "poziv sa literalom od 32 znaka: const std::string& " << zaStr << " alokacija, string_view "
              << alokacija << '\n';

    std::string s = dug;
    alokacija = 0;
    std::string deo1 = s.substr(7, 20);
    int zaSubstr = alokacija;
    alokacija = 0;
    std::string_view deo2 = std::string_view(s).substr(7, 20);
    std::cout << "substr 20 znakova: string " << zaSubstr << " alokacija, string_view " << alokacija << '\n';
    (void)deo1;
    (void)deo2;

    std::string konfig = "temp=21;vlaga=40;pritisak=1013";
    std::vector<std::string_view> delovi;
    delovi.reserve(8);
    alokacija = 0;
    for (auto d : podeli(konfig, ';')) delovi.push_back(d);
    std::cout << "podeli na " << delovi.size() << " dela:";
    for (auto d : delovi) std::cout << " [" << d << ']';
    std::cout << " -- alokacija samo za vector: " << alokacija << '\n';
}

// ---------------------------------------------------------------- 3
void sekcija3() {
    std::cout << "\n== 3. string_view nije string: kraj i životni vek\n";
    std::string s = "temperatura";
    std::string_view pocetak = std::string_view(s).substr(0, 4);   // "temp"
    std::cout << "pogled: " << pocetak << ", size " << pocetak.size() << '\n';
    // data() pokazuje na početak -- i NEMA '\0' posle "temp" (zadatak z3):
    std::cout << "C string od data(): " << pocetak.data() << " (ceo ostatak originala)\n";
    std::cout << "ispravno za C API: " << std::string(pocetak).c_str() << '\n';
    // Pogled važi dok važi original i dok se original ne menja (ub/u01, u02).
}

// ---------------------------------------------------------------- 4
void sekcija4() {
    std::cout << "\n== 4. std::filesystem::path\n";
    fs::path p = fs::path("merenja") / "2024" / "hala3.temp.log";   // / spaja sa separatorom
    std::cout << "putanja: " << p.generic_string() << '\n';
    std::cout << "parent_path " << p.parent_path().generic_string() << ", filename " << p.filename()
              << ", stem " << p.stem() << ", extension " << p.extension() << '\n';
    fs::path q = p;
    q.replace_extension(".csv");
    std::cout << "replace_extension: " << q.filename().generic_string() << '\n';
    std::cout << "delovi:";
    for (const auto& deo : p) std::cout << " [" << deo.generic_string() << ']';
    std::cout << '\n';
    fs::path neuredna = "merenja/./2024/../2025/a.log";
    std::cout << "lexically_normal: " << neuredna.lexically_normal().generic_string() << '\n';
    std::cout << "lexically_relative: "
              << fs::path("merenja/2025/a.log").lexically_relative("merenja/2024").generic_string() << '\n';
    std::cout << "is_absolute(\"/tmp\") = " << fs::path("/tmp").is_absolute()
              << ", has_extension(\"README\") = " << fs::path("README").has_extension() << '\n';
}

// ---------------------------------------------------------------- 5
fs::path napraviKoren() {
    auto broj = std::chrono::steady_clock::now().time_since_epoch().count();
    fs::path koren = fs::temp_directory_path() / ("mcpp_s23_" + std::to_string(broj));
    fs::create_directories(koren);
    return koren;
}

void upisi(const fs::path& p, std::string_view sadrzaj) {
    std::ofstream out(p);
    out << sadrzaj;
}

void sekcija5(const fs::path& koren) {
    std::cout << "\n== 5. directory_entry i status\n";
    upisi(koren / "a.log", "21.5\n22.0\n");             // 10 bajtova
    fs::create_directory(koren / "arhiva");
    for (const char* ime : {"a.log", "arhiva", "nema.txt"}) {
        fs::directory_entry e(koren / ime);
        std::cout << ime << ": exists " << e.exists() << ", fajl " << e.is_regular_file() << ", direktorijum "
                  << e.is_directory();
        if (e.is_regular_file()) std::cout << ", " << e.file_size() << " B";
        std::cout << '\n';
    }
}

// ---------------------------------------------------------------- 6
std::vector<std::string> sadrzaj(const fs::path& koren, bool rekurzivno) {
    std::vector<std::string> imena;
    if (rekurzivno) {
        for (const auto& e : fs::recursive_directory_iterator(koren))
            imena.push_back(e.path().lexically_relative(koren).generic_string() + (e.is_directory() ? "/" : ""));
    } else {
        for (const auto& e : fs::directory_iterator(koren))
            imena.push_back(e.path().filename().generic_string() + (e.is_directory() ? "/" : ""));
    }
    std::sort(imena.begin(), imena.end());               // redosled iteratora nije određen
    return imena;
}

void ispisiListu(const char* naslov, const std::vector<std::string>& imena) {
    std::cout << naslov << ':';
    for (const auto& i : imena) std::cout << ' ' << i;
    std::cout << '\n';
}

void sekcija6(const fs::path& koren) {
    std::cout << "\n== 6. funkcije za direktorijume i fajlove\n";
    fs::create_directories(koren / "arhiva" / "2024");   // i svi međudirektorijumi
    upisi(koren / "b.log", "40\n");
    fs::copy_file(koren / "a.log", koren / "arhiva" / "a.log");
    fs::rename(koren / "b.log", koren / "arhiva" / "2024" / "b.log");
    ispisiListu("directory_iterator (sortirano)", sadrzaj(koren, false));
    ispisiListu("recursive_directory_iterator", sadrzaj(koren, true));

    std::error_code ec;                                  // verzija bez izuzetka
    auto velicina = fs::file_size(koren / "nema.log", ec);
    std::cout << "file_size nepostojećeg sa error_code: greška " << static_cast<bool>(ec)
              << ", vraćeno == static_cast<uintmax_t>(-1): " << (velicina == static_cast<std::uintmax_t>(-1)) << '\n';
    try {
        fs::remove(koren / "arhiva");                    // nije prazan
    } catch (const fs::filesystem_error& e) {
        std::cout << "remove nepraznog direktorijuma: filesystem_error, kod "
                  << (e.code() == std::errc::directory_not_empty) << '\n';
    }
    std::cout << "remove_all(arhiva) obrisao: " << fs::remove_all(koren / "arhiva") << " stavke\n";
}

// ---------------------------------------------------------------- 7
std::string rwx(fs::perms p) {
    std::string s;
    const fs::perms bitovi[] = {fs::perms::owner_read,  fs::perms::owner_write,  fs::perms::owner_exec,
                                fs::perms::group_read,  fs::perms::group_write,  fs::perms::group_exec,
                                fs::perms::others_read, fs::perms::others_write, fs::perms::others_exec};
    const char slova[] = "rwxrwxrwx";
    for (int i = 0; i < 9; ++i) s += (p & bitovi[i]) != fs::perms::none ? slova[i] : '-';
    return s;
}

void sekcija7(const fs::path& koren) {
    std::cout << "\n== 7. dozvole\n";
    fs::path f = koren / "a.log";
    fs::permissions(f, fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read,
                    fs::perm_options::replace);
    std::cout << "replace rw-r-----: " << rwx(fs::status(f).permissions()) << '\n';
    fs::permissions(f, fs::perms::others_read, fs::perm_options::add);
    std::cout << "add others_read:   " << rwx(fs::status(f).permissions()) << '\n';
    fs::permissions(f, fs::perms::owner_write, fs::perm_options::remove);
    std::cout << "remove owner_write: " << rwx(fs::status(f).permissions()) << '\n';
    std::cout << "oktalno: " << std::oct << static_cast<int>(fs::status(f).permissions()) << std::dec << '\n';
}

int main() {
    std::cout << std::boolalpha;
    sekcija1();
    sekcija2();
    sekcija3();
    sekcija4();
    fs::path koren = napraviKoren();
    sekcija5(koren);
    sekcija6(koren);
    sekcija7(koren);
    fs::remove_all(koren);
    std::cout << "\nprivremeni direktorijum obrisan: " << !fs::exists(koren) << '\n';
}
