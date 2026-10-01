// Korak 1 -- ZADATAK: "RAII wrapper za C resurs" (FILE*)
//   ./build.sh roadmap/step-1-c-to-cpp/task/raii_file.cpp
// Uputstvo: roadmap/step-1-c-to-cpp/notes.md, deo "Zadatak".
// Rešenje: ../solutions/raii_file.cpp (otvori tek kad tvoj izlaz bude isti
// kao blok EXPECTED OUTPUT na dnu fajla).
//
// Fajl se kompajlira i ovakav. Napiši klasu File, pa otkomentariši korake
// u main()-u jedan po jedan.
//
// Korak A: class File
//   - File(const char* path, const char* mode): fopen; ako ne uspe ->
//     throw std::runtime_error{"cannot open " + path}
//     (pitanje za tebe: da li će se tada pozvati destruktor? zašto?)
//   - ~File(): fclose
//   - copy ctor, copy dodela, move ctor, move dodela: = delete
//   - std::FILE* get() const noexcept
//   - const std::string& path() const noexcept
//   - void write_line(const char* text)   -- fprintf(f_, "%s\n", text)
//   - static int open_count()             -- koliko je File objekata živo
//     (static inline int član, ++ u konstruktoru, -- u destruktoru)
//   - u konstruktoru ispiši "  [open  <path>]", u destruktoru "  [close <path>]"
//     (posle "open" su DVA razmaka, da se poravna sa "close")
// Korak B: write_report() -- rani izlaz (dva return-a, nijedan fclose)
// Korak C: copy_and_fail() -- dva fajla, izuzetak posred rada
// Korak D: count_lines(const File&) -- zašto const File&, a ne File ili File*?
// Korak E: probaj -DTRY_COPY: mora da padne pri kompajliranju. Pročitaj poruku.

#include <cstdio>
#include <stdexcept>
#include <string>

// TODO korak A: class File

// TODO korak B
// int write_report(const char* path, int lines) {
//     File f{path, "w"};
//     if (lines <= 0) {
//         std::printf("  nothing to write, early return\n");
//         return -1;
//     }
//     for (int i = 0; i < lines; ++i) {
//         if (i == 2) {
//             std::printf("  limit reached, early return\n");
//             return i;
//         }
//         f.write_line("data");
//     }
//     return lines;
// }

// TODO korak C: otvori `from` za čitanje i `to` za pisanje, prepiši red po
// red (fgets/fputs preko get()), ispiši "  copied, now throwing\n" i baci
// std::runtime_error{"validation failed after copy"}.
// void copy_and_fail(const char* from, const char* to) { }

// TODO korak D: rewind(f.get()), pa broj redova preko fgets.
// int count_lines(const File& f) { }

int main() {
    const char* a = "step1_a.txt";
    const char* b = "step1_b.txt";
    (void)a;
    (void)b;

    // Korak A -- otkomentariši:
    // std::printf("== 1. normal scope\n");
    // {
    //     File f{a, "w"};
    //     f.write_line("first");
    //     f.write_line("second");
    //     std::printf("  leaving scope\n");
    // }

    // Korak B -- otkomentariši:
    // std::printf("== 2. early return\n");
    // std::printf("  write_report(0) = %d\n", write_report(b, 0));
    // std::printf("  write_report(5) = %d\n", write_report(b, 5));

    // Korak C -- otkomentariši:
    // std::printf("== 3. exception\n");
    // try {
    //     copy_and_fail(a, b);
    // } catch (const std::exception& e) {
    //     std::printf("  caught: %s (open files: %d)\n", e.what(), File::open_count());
    // }
    // std::printf("== 4. constructor failure\n");
    // try {
    //     File missing{"no_such_dir/x.txt", "r"};
    //     std::printf("  unreachable\n");
    // } catch (const std::exception& e) {
    //     std::printf("  caught: %s (open files: %d)\n", e.what(), File::open_count());
    // }

    // Korak D -- otkomentariši:
    // std::printf("== 5. const File&\n");
    // {
    //     const File f{b, "r"};
    //     std::printf("  %s has %d lines\n", f.path().c_str(), count_lines(f));
    // }

#ifdef TRY_COPY
    File f1{a, "r"};
    File f2 = f1;   // mora da bude greška kompajlera
#endif

    // Na kraju -- otkomentariši:
    // std::remove(a);
    // std::remove(b);
    // std::printf("== done, open files: %d\n", File::open_count());
}

/* EXPECTED OUTPUT
== 1. normal scope
  [open  step1_a.txt]
  leaving scope
  [close step1_a.txt]
== 2. early return
  [open  step1_b.txt]
  nothing to write, early return
  [close step1_b.txt]
  write_report(0) = -1
  [open  step1_b.txt]
  limit reached, early return
  [close step1_b.txt]
  write_report(5) = 2
== 3. exception
  [open  step1_a.txt]
  [open  step1_b.txt]
  copied, now throwing
  [close step1_b.txt]
  [close step1_a.txt]
  caught: validation failed after copy (open files: 0)
== 4. constructor failure
  caught: cannot open no_such_dir/x.txt (open files: 0)
== 5. const File&
  [open  step1_b.txt]
  step1_b.txt has 2 lines
  [close step1_b.txt]
== done, open files: 0
*/
