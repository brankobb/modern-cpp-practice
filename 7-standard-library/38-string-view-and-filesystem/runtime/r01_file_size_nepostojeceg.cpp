// EXPECT-RUN: filesystem error: cannot get file size: No such file or directory
// POGREŠNO: funkcije iz std::filesystem bez error_code parametra bacaju
// std::filesystem::filesystem_error kad operacija ne uspe (fajl ne
// postoji, nema dozvole...). Neuhvaćen -- terminate. what() sadrži i
// putanju.
// Ispravno: try/catch (const std::filesystem::filesystem_error& e), ili
// verzija sa std::error_code (ne baca; main.cpp, sekcija 6):
//   std::error_code ec; auto n = fs::file_size(p, ec); if (ec) ...
#include <filesystem>
int main() { return static_cast<int>(std::filesystem::file_size("nepostojeci/merenja.log")); }
