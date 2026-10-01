// Korak 1 -- REŠENJE zadatka "RAII wrapper za C resurs" (FILE*).
//   ./build.sh roadmap/step-1-c-to-cpp/solutions/raii_file.cpp
//   ./build.sh roadmap/step-1-c-to-cpp/solutions/raii_file.cpp -DTRY_COPY   -- mora da padne pri kompajliranju
//
// Šta da uporediš sa svojim rešenjem (ne samo izlaz):
//   - destruktor proverava f_ != nullptr? (ovde ne mora, jer konstruktor
//     baca ako fopen ne uspe -- objekat sa f_ == nullptr nikad ne postoji;
//     ali u Koraku 3, posle move-a, MORAĆE)
//   - get() je const: pristup ne menja objekat (iako se kroz vraćeni
//     FILE* može pisati -- const štiti objekat, ne resurs na koji pokazuje)
//   - kopiranje i move su = delete: File ne može ni da se kopira ni da se
//     pomeri (move dodajemo u Koraku 3)

#include <cstdio>
#include <stdexcept>
#include <string>

class File {
public:
    File(const char* path, const char* mode) : f_{std::fopen(path, mode)}, path_{path} {
        if (f_ == nullptr) {
            // Konstruktor nije završio => objekat ne postoji => destruktor se NEĆE pozvati.
            // Zato se ovde ništa ne čisti: nije ni dobijeno.
            throw std::runtime_error{"cannot open " + path_};
        }
        ++open_count_;
        std::printf("  [open  %s]\n", path_.c_str());
    }

    ~File() {
        std::fclose(f_);
        --open_count_;
        std::printf("  [close %s]\n", path_.c_str());
    }

    File(const File&) = delete;
    File& operator=(const File&) = delete;
    File(File&&) = delete;               // Korak 3: ovde dolazi pravi move
    File& operator=(File&&) = delete;

    std::FILE* get() const noexcept { return f_; }
    const std::string& path() const noexcept { return path_; }

    void write_line(const char* text) {  // nije const: menja stanje fajla
        if (std::fprintf(f_, "%s\n", text) < 0) throw std::runtime_error{"write failed: " + path_};
    }

    static int open_count() noexcept { return open_count_; }

private:
    std::FILE* f_;
    std::string path_;
    static inline int open_count_ = 0;   // C++17: definicija u klasi
};

// Rani izlaz: tri return-a, nijedan fclose.
int write_report(const char* path, int lines) {
    File f{path, "w"};
    if (lines <= 0) {
        std::printf("  nothing to write, early return\n");
        return -1;
    }
    for (int i = 0; i < lines; ++i) {
        if (i == 2) {
            std::printf("  limit reached, early return\n");
            return i;
        }
        f.write_line("data");
    }
    return lines;
}

// Izuzetak posred rada sa dva resursa.
void copy_and_fail(const char* from, const char* to) {
    File in{from, "r"};
    File out{to, "w"};
    char buf[64];
    while (std::fgets(buf, sizeof buf, in.get()) != nullptr) {
        std::fputs(buf, out.get());
    }
    std::printf("  copied, now throwing\n");
    throw std::runtime_error{"validation failed after copy"};
    // Redosled zatvaranja: out pa in (obrnuto od otvaranja).
}

int count_lines(const File& f) {         // const File&: bez kopije, bez null-a, bez izmene objekta
    int n = 0;
    char buf[64];
    std::rewind(f.get());
    while (std::fgets(buf, sizeof buf, f.get()) != nullptr) ++n;
    return n;
}

int main() {
    const char* a = "step1_a.txt";
    const char* b = "step1_b.txt";

    std::printf("== 1. normal scope\n");
    {
        File f{a, "w"};
        f.write_line("first");
        f.write_line("second");
        std::printf("  leaving scope\n");
    }

    std::printf("== 2. early return\n");
    std::printf("  write_report(0) = %d\n", write_report(b, 0));
    std::printf("  write_report(5) = %d\n", write_report(b, 5));

    std::printf("== 3. exception\n");
    try {
        copy_and_fail(a, b);
    } catch (const std::exception& e) {
        std::printf("  caught: %s (open files: %d)\n", e.what(), File::open_count());
    }

    std::printf("== 4. constructor failure\n");
    try {
        File missing{"no_such_dir/x.txt", "r"};
        std::printf("  unreachable\n");
    } catch (const std::exception& e) {
        std::printf("  caught: %s (open files: %d)\n", e.what(), File::open_count());
    }

    std::printf("== 5. const File&\n");
    {
        const File f{b, "r"};
        std::printf("  %s has %d lines\n", f.path().c_str(), count_lines(f));
        // f.write_line("x");            // ne kompajlira se: write_line nije const
    }

#ifdef TRY_COPY
    File f1{a, "r"};
    File f2 = f1;                        // greška: use of deleted function 'File::File(const File&)'
#endif

    std::remove(a);
    std::remove(b);
    std::printf("== done, open files: %d\n", File::open_count());
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
