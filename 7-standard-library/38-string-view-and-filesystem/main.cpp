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
// ./check_cases.sh 7-standard-library/38-string-view-and-filesystem  proverava sve.

namespace fs = std::filesystem;
using namespace std::string_view_literals;

static int allocations = 0;   // brojač za sekciju 2
void* operator new(std::size_t n) {
    ++allocations;
    if (void* p = std::malloc(n)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

// ---------------------------------------------------------------- 1
void section1() {
    std::cout << "\n== 1. string_view: a view of someone else's characters\n";
    std::string s = "temperature";
    const char arr[] = {'r', 'a', 'i', 'n'};       // bez '\0'
    std::string_view a = "pressure";                    // literal
    std::string_view b = s;                             // std::string
    std::string_view c(arr, sizeof(arr));               // pokazivač + dužina
    auto d = "voltage"sv;                                 // sv literal
    std::cout << a << ' ' << b << ' ' << c << ' ' << d << '\n';
    std::cout << "sizeof(string_view) = " << sizeof(std::string_view) << " (pointer + length)\n";

    std::string_view t = b.substr(0, 5);                // "tempe" -- bez kopiranja
    std::cout << "substr(0, 5): " << t << ", same memory as s: " << (t.data() == s.data()) << '\n';
    std::string_view line = "  [temp] 21.5  ";
    line.remove_prefix(line.find_first_not_of(' '));      // skida sa početka POGLEDA
    line.remove_suffix(line.size() - line.find_last_not_of(' ') - 1);
    std::cout << "after remove_prefix/suffix: \"" << line << "\"\n";
    std::cout << "find(']') = " << line.find(']') << ", comparison with \"[temp] 21.5\": "
              << (line == "[temp] 21.5") << '\n';
    std::string copy(line);                            // u std::string samo EKSPLICITNO (errors/e01)
    std::cout << "std::string(line).size() = " << copy.size() << '\n';
}

// ---------------------------------------------------------------- 2
std::size_t lengthStr(const std::string& s) { return s.size(); }
std::size_t lengthSv(std::string_view s) { return s.size(); }

std::vector<std::string_view> split(std::string_view text, char sep) {
    std::vector<std::string_view> parts;
    while (true) {
        auto pos = text.find(sep);
        parts.push_back(text.substr(0, pos));
        if (pos == std::string_view::npos) break;
        text.remove_prefix(pos + 1);
    }
    return parts;
}

void section2() {
    std::cout << "\n== 2. string_view as a parameter: no copies\n";
    const char* longText = "temperature sensor in hall no. 3";   // duže od SSO bafera (15 znakova)
    allocations = 0;
    lengthStr(longText);                                     // pravi privremeni std::string
    int forStr = allocations;
    allocations = 0;
    lengthSv(longText);
    std::cout << "call with a 32-character literal: const std::string& " << forStr << " allocations, string_view "
              << allocations << '\n';

    std::string s = longText;
    allocations = 0;
    std::string part1 = s.substr(7, 20);
    int forSubstr = allocations;
    allocations = 0;
    std::string_view part2 = std::string_view(s).substr(7, 20);
    std::cout << "substr of 20 characters: string " << forSubstr << " allocations, string_view " << allocations << '\n';
    (void)part1;
    (void)part2;

    std::string config = "temp=21;humidity=40;pressure=1013";
    std::vector<std::string_view> parts;
    parts.reserve(8);
    allocations = 0;
    for (auto d : split(config, ';')) parts.push_back(d);
    std::cout << "split into " << parts.size() << " parts:";
    for (auto d : parts) std::cout << " [" << d << ']';
    std::cout << " -- allocations only for the vector: " << allocations << '\n';
}

// ---------------------------------------------------------------- 3
void section3() {
    std::cout << "\n== 3. string_view is not a string: end and lifetime\n";
    std::string s = "temperature";
    std::string_view prefix = std::string_view(s).substr(0, 4);   // "temp"
    std::cout << "view: " << prefix << ", size " << prefix.size() << '\n';
    // data() pokazuje na početak -- i NEMA '\0' posle "temp" (zadatak ex3):
    std::cout << "C string from data(): " << prefix.data() << " (the whole rest of the original)\n";
    std::cout << "correct for a C API: " << std::string(prefix).c_str() << '\n';
    // Pogled važi dok važi original i dok se original ne menja (ub/u01, u02).
}

// ---------------------------------------------------------------- 4
void section4() {
    std::cout << "\n== 4. std::filesystem::path\n";
    fs::path p = fs::path("readings") / "2024" / "hall3.temp.log";   // / spaja sa separatorom
    std::cout << "path: " << p.generic_string() << '\n';
    std::cout << "parent_path " << p.parent_path().generic_string() << ", filename " << p.filename()
              << ", stem " << p.stem() << ", extension " << p.extension() << '\n';
    fs::path q = p;
    q.replace_extension(".csv");
    std::cout << "replace_extension: " << q.filename().generic_string() << '\n';
    std::cout << "parts:";
    for (const auto& part : p) std::cout << " [" << part.generic_string() << ']';
    std::cout << '\n';
    fs::path messy = "readings/./2024/../2025/a.log";
    std::cout << "lexically_normal: " << messy.lexically_normal().generic_string() << '\n';
    std::cout << "lexically_relative: "
              << fs::path("readings/2025/a.log").lexically_relative("readings/2024").generic_string() << '\n';
    std::cout << "is_absolute(\"/tmp\") = " << fs::path("/tmp").is_absolute()
              << ", has_extension(\"README\") = " << fs::path("README").has_extension() << '\n';
}

// ---------------------------------------------------------------- 5
fs::path makeRoot() {
    auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    fs::path root = fs::temp_directory_path() / ("mcpp_s23_" + std::to_string(stamp));
    fs::create_directories(root);
    return root;
}

void writeFile(const fs::path& p, std::string_view content) {
    std::ofstream out(p);
    out << content;
}

void section5(const fs::path& root) {
    std::cout << "\n== 5. directory_entry and status\n";
    writeFile(root / "a.log", "21.5\n22.0\n");             // 10 bajtova
    fs::create_directory(root / "archive");
    for (const char* name : {"a.log", "archive", "missing.txt"}) {
        fs::directory_entry e(root / name);
        std::cout << name << ": exists " << e.exists() << ", file " << e.is_regular_file() << ", directory "
                  << e.is_directory();
        if (e.is_regular_file()) std::cout << ", " << e.file_size() << " B";
        std::cout << '\n';
    }
}

// ---------------------------------------------------------------- 6
std::vector<std::string> listing(const fs::path& root, bool recursive) {
    std::vector<std::string> names;
    if (recursive) {
        for (const auto& e : fs::recursive_directory_iterator(root))
            names.push_back(e.path().lexically_relative(root).generic_string() + (e.is_directory() ? "/" : ""));
    } else {
        for (const auto& e : fs::directory_iterator(root))
            names.push_back(e.path().filename().generic_string() + (e.is_directory() ? "/" : ""));
    }
    std::sort(names.begin(), names.end());               // redosled iteratora nije određen
    return names;
}

void printList(const char* title, const std::vector<std::string>& names) {
    std::cout << title << ':';
    for (const auto& i : names) std::cout << ' ' << i;
    std::cout << '\n';
}

void section6(const fs::path& root) {
    std::cout << "\n== 6. functions for directories and files\n";
    fs::create_directories(root / "archive" / "2024");   // i svi međudirektorijumi
    writeFile(root / "b.log", "40\n");
    fs::copy_file(root / "a.log", root / "archive" / "a.log");
    fs::rename(root / "b.log", root / "archive" / "2024" / "b.log");
    printList("directory_iterator (sorted)", listing(root, false));
    printList("recursive_directory_iterator", listing(root, true));

    std::error_code ec;                                  // verzija bez izuzetka
    auto size = fs::file_size(root / "missing.log", ec);
    std::cout << "file_size of a missing file with error_code: error " << static_cast<bool>(ec)
              << ", returned == static_cast<uintmax_t>(-1): " << (size == static_cast<std::uintmax_t>(-1)) << '\n';
    try {
        fs::remove(root / "archive");                    // nije prazan
    } catch (const fs::filesystem_error& e) {
        std::cout << "remove of a non-empty directory: filesystem_error, code "
                  << (e.code() == std::errc::directory_not_empty) << '\n';
    }
    std::cout << "remove_all(archive) erased: " << fs::remove_all(root / "archive") << " entries\n";
}

// ---------------------------------------------------------------- 7
std::string rwx(fs::perms p) {
    std::string s;
    const fs::perms bits[] = {fs::perms::owner_read,  fs::perms::owner_write,  fs::perms::owner_exec,
                                fs::perms::group_read,  fs::perms::group_write,  fs::perms::group_exec,
                                fs::perms::others_read, fs::perms::others_write, fs::perms::others_exec};
    const char letters[] = "rwxrwxrwx";
    for (int i = 0; i < 9; ++i) s += (p & bits[i]) != fs::perms::none ? letters[i] : '-';
    return s;
}

void section7(const fs::path& root) {
    std::cout << "\n== 7. permissions\n";
    fs::path f = root / "a.log";
    fs::permissions(f, fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read,
                    fs::perm_options::replace);
    std::cout << "replace rw-r-----: " << rwx(fs::status(f).permissions()) << '\n';
    fs::permissions(f, fs::perms::others_read, fs::perm_options::add);
    std::cout << "add others_read:   " << rwx(fs::status(f).permissions()) << '\n';
    fs::permissions(f, fs::perms::owner_write, fs::perm_options::remove);
    std::cout << "remove owner_write: " << rwx(fs::status(f).permissions()) << '\n';
    std::cout << "octal: " << std::oct << static_cast<int>(fs::status(f).permissions()) << std::dec << '\n';
}

int main() {
    std::cout << std::boolalpha;
    section1();
    section2();
    section3();
    section4();
    fs::path root = makeRoot();
    section5(root);
    section6(root);
    section7(root);
    fs::remove_all(root);
    std::cout << "\ntemporary directory removed: " << !fs::exists(root) << '\n';
}
