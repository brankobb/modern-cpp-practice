#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <vector>

// Dinamička memorija: malloc/free, new/delete, new[]/delete[], 2D nizovi --
// ISPRAVNI slučajevi. Sve se kompajlira i radi bez ASan/UBSan prijava
// (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 1-language-basics/13-dynamic-memory  proverava oba.

// Klasa koja javlja kad se konstruiše i uništi, da se vidi šta new/delete
// rade, a šta malloc/free ne rade.
struct Tracer {
    static int nextId;
    int id;
    Tracer() : id(++nextId) { std::cout << "Tracer" << id << "() "; }
    ~Tracer() { std::cout << "~Tracer" << id << "() "; }
};
int Tracer::nextId = 0;

// ---------------------------------------------------------------- 1
int* makeOnHeap(int value) {
    int local = value;     // automatic: nestaje na kraju funkcije
    return new int(local); // dynamic: živi dok ga neko ne obriše
}

void s01_storageDuration() {
    std::cout << "-- 1. zašto dinamička memorija --\n";
    int* p = makeOnHeap(7);
    std::cout << "  objekat napravljen u funkciji živi i posle nje: *p = " << *p << "\n";
    delete p;

    // Veličina poznata tek u toku izvršavanja: int arr[n] je VLA i nije C++
    // (lekcija 05), a new int[n] jeste.
    int n = static_cast<int>(std::string("hello").size());
    int* arr = new int[n]();
    std::cout << "  new int[n] sa n=" << n << " iz runtime-a: arr[" << n - 1 << "] = " << arr[n - 1] << "\n";
    delete[] arr;
}

// ---------------------------------------------------------------- 2
struct Named {
    std::string name;
    int id;
};

void s02_mallocFree() {
    std::cout << "-- 2. malloc/free (C) --\n";
    // Vraća void* (u C++ obavezan cast, errors/e01), veličina je u BAJTOVIMA,
    // memorija NIJE inicijalizovana.
    int* p = static_cast<int*>(std::malloc(3 * sizeof(int)));
    if (p == nullptr) return; // malloc javlja neuspeh sa nullptr, ne izuzetkom
    p[0] = 1; p[1] = 2; p[2] = 3; // prvo upiši, pa tek onda čitaj
    std::cout << "  malloc(3 * sizeof(int)): " << p[0] << " " << p[1] << " " << p[2] << "\n";

    // realloc: nova veličina; stari sadržaj ostaje, ali adresa može da se
    // promeni. Stari pokazivač posle toga ne sme da se koristi (ub/u09).
    int* bigger = static_cast<int*>(std::realloc(p, 5 * sizeof(int)));
    if (bigger == nullptr) { std::free(p); return; } // p je i dalje važeći ako realloc ne uspe
    p = bigger;
    p[3] = 4; p[4] = 5;
    std::cout << "  realloc na 5: " << p[0] << " " << p[1] << " " << p[2] << " " << p[3] << " " << p[4] << "\n";
    std::free(p);

    int* zeros = static_cast<int*>(std::calloc(3, sizeof(int))); // calloc: nule
    if (zeros == nullptr) return;
    std::cout << "  calloc(3, sizeof(int)): " << zeros[0] << " " << zeros[1] << " " << zeros[2] << "\n";
    std::free(zeros);
    std::free(nullptr); // dozvoljeno, ne radi ništa (kao i delete nullptr)

    // malloc NE poziva konstruktor. Za int to nije problem, ali Named ima
    // std::string, koji mora da se konstruiše pre upotrebe (ub/u06).
    // Ako baš moraš da koristiš malloc memoriju za objekat: placement new
    // pozove konstruktor, a destruktor se onda poziva RUČNO.
    void* raw = std::malloc(sizeof(Named));
    if (raw == nullptr) return;
    Named* obj = new (raw) Named{"Ana", 1}; // placement new: samo konstrukcija, bez alokacije
    std::cout << "  placement new u malloc memoriji: " << obj->name << " " << obj->id << "\n";
    obj->~Named();  // ručni poziv destruktora
    std::free(raw); // pa tek onda oslobađanje
}

// ---------------------------------------------------------------- 3
struct Point {
    int x;
    int y;
};

void s03_newDelete() {
    std::cout << "-- 3. new/delete: alokacija + konstruktor --\n";
    std::cout << "  new Tracer:    ";
    Tracer* t = new Tracer; // 1) operator new alocira  2) poziva se konstruktor
    std::cout << "\n  delete:        ";
    delete t;               // 1) poziva se destruktor  2) operator delete oslobađa
    std::cout << "\n  malloc/free:   ";
    void* m = std::malloc(sizeof(Tracer)); // ništa se ne ispisuje: nema konstruktora
    std::free(m);                          // ni destruktora
    std::cout << "(ništa -- malloc/free ne znaju za konstruktore)\n";

    // Oblici inicijalizacije su isti kao za obične promenljive (lekcija 03).
    int* a = new int(5);          // direktna
    int* b = new int{5};          // lista
    int* c = new int();           // value-init -> 0
    Point* pt = new Point{1, 2};  // agregat
    // new int (bez zagrada) je default-init: vrednost je NEODREĐENA, a čitanje je UB.
    std::cout << "  new int(5)=" << *a << " new int{5}=" << *b << " new int()=" << *c
              << " new Point{1, 2}=(" << pt->x << ", " << pt->y << ")\n";
    delete a;
    delete b;
    delete c;
    delete pt;

    Point* none = nullptr;
    delete none; // dozvoljeno, ne radi ništa -- ne treba "if (p) delete p;"
    std::cout << "  delete nullptr: dozvoljeno\n";
}

// ---------------------------------------------------------------- 4
void s04_allocationFailure() {
    std::cout << "-- 4. neuspela alokacija --\n";
    // new ne vraća nullptr, nego baca std::bad_alloc. Ovde broj elemenata
    // puta sizeof(int) ne staje u size_t. Standard traži
    // std::bad_array_new_length (izveden iz bad_alloc); g++ baca njega, a
    // clang baca bad_alloc. Hvatanje std::bad_alloc radi na oba.
    std::size_t huge = SIZE_MAX / 2 + 1;
    try {
        int* p = new int[huge];
        delete[] p;
    } catch (const std::bad_alloc&) {
        std::cout << "  new int[ogroman broj] -> std::bad_alloc (ne nullptr)\n";
    }
}

// ---------------------------------------------------------------- 5
struct Fragile {
    static int created;
    int id;
    Fragile() : id(created + 1) {
        if (id == 3) throw std::runtime_error("treći konstruktor ne uspeva");
        ++created;
        std::cout << "Fragile" << id << "() ";
    }
    ~Fragile() { std::cout << "~Fragile" << id << "() "; }
};
int Fragile::created = 0;

void s05_arrayNew() {
    std::cout << "-- 5. new[]/delete[] --\n";
    std::cout << "  new Tracer[3]: ";
    Tracer* ts = new Tracer[3]; // konstruktor za svaki element, redom
    std::cout << "\n  delete[]:      ";
    delete[] ts;                // destruktor za svaki element, OBRNUTIM redom
    std::cout << "\n";

    int* zeros = new int[4]();          // () -> sve nule
    int* partial = new int[5]{1, 2};    // ostatak se value-inicijalizuje -> 0
    int* deduced = new int[]{7, 8, 9};  // veličina iz inicijalizatora
    std::cout << "  new int[4]()=" << zeros[0] << zeros[1] << zeros[2] << zeros[3]
              << " new int[5]{1, 2}=" << partial[0] << partial[1] << partial[2] << partial[3] << partial[4]
              << " new int[]{7, 8, 9}=" << deduced[0] << deduced[1] << deduced[2] << "\n";
    delete[] zeros;
    delete[] partial;
    delete[] deduced;

    // Niz dužine 0 je dozvoljen: pokazivač nije null, ali nema elemenata.
    int* empty = new int[0];
    std::cout << "  new int[0]: " << (empty != nullptr ? "nije nullptr" : "nullptr") << ", ali *empty je UB\n";
    delete[] empty;

    // Ako konstruktor jednog elementa baci izuzetak, već napravljeni elementi
    // se unište i memorija se oslobodi sama. Nema curenja.
    std::cout << "  new Fragile[3]: ";
    try {
        Fragile* fs = new Fragile[3];
        delete[] fs;
    } catch (const std::runtime_error& e) {
        std::cout << "| izuzetak: " << e.what() << "\n";
    }
}

// ---------------------------------------------------------------- 6
constexpr int kCols = 4;

void s06_twoDimensional() {
    std::cout << "-- 6. 2D nizovi: četiri načina --\n";
    const int rows = 3;
    const int cols = kCols;

    // (a) niz pokazivača na redove: rows + 1 alokacija, redovi razbacani po memoriji.
    int** jagged = new int*[rows];
    for (int r = 0; r < rows; ++r) jagged[r] = new int[cols]();
    jagged[1][2] = 5;
    // Redovi su posebne alokacije: gde je koji u memoriji, ne zna se.
    std::cout << "  (a) new int*[rows] + new int[cols] po redu: m[1][2]=" << jagged[1][2]
              << ", alokacija=" << rows + 1 << "\n";
    for (int r = 0; r < rows; ++r) delete[] jagged[r]; // prvo redovi (ub/u07 ako se preskoči)
    delete[] jagged;                                     // pa niz pokazivača

    // (b) jedan blok, indeks se računa ručno: r * cols + c.
    int* flat = new int[rows * cols]();
    flat[1 * cols + 2] = 5; // red 1, kolona 2 (zamenjen red i kolona: ub/u08)
    std::cout << "  (b) new int[rows * cols], m[r * cols + c]: m[1][2]=" << flat[1 * cols + 2] << ", alokacija=1\n";
    delete[] flat;

    // (c) broj kolona poznat pri kompajliranju: pravi 2D niz na heap-u.
    // Tip je int (*)[kCols], NE int** (errors/e07). Kolone moraju biti
    // konstanta (errors/e05).
    int (*fixed)[kCols] = new int[rows][kCols]();
    fixed[1][2] = 5;
    // Red je tip int[kCols], pa je fixed + 1 sledeći red, kCols * sizeof(int) bajtova dalje.
    std::cout << "  (c) new int[rows][kCols]: m[1][2]=" << fixed[1][2] << ", alokacija=1, sizeof(fixed[0])="
              << sizeof(fixed[0]) << " (ceo red)\n";
    delete[] fixed;

    // (d) moderno: std::vector oslobađa sam, i kad izuzetak prekine funkciju.
    std::vector<std::vector<int>> nested(rows, std::vector<int>(cols, 0));
    nested[1][2] = 5;
    std::vector<int> grid(rows * cols, 0); // jedan blok, kao (b)
    grid[1 * cols + 2] = 5;
    std::cout << "  (d) vector<vector<int>>: m[1][2]=" << nested[1][2] << "; vector<int>(rows * cols): m[1][2]="
              << grid[1 * cols + 2] << " -- bez delete\n";
}

// ---------------------------------------------------------------- 7
void s07_modern() {
    std::cout << "-- 7. moderni C++: bez ručnog delete --\n";
    auto one = std::make_unique<int>(42);      // umesto new int(42)
    auto many = std::make_unique<int[]>(3);    // umesto new int[3]() -- nule
    many[1] = 7;
    std::vector<int> v(3);                     // obično je ovo i najbolje rešenje
    std::cout << "  make_unique<int>(42)=" << *one << " make_unique<int[]>(3)=" << many[0] << many[1] << many[2]
              << " vector<int>(3)=" << v[0] << v[1] << v[2] << "\n";
    std::cout << "  (sve se oslobađa samo na kraju scope-a)\n";
}

int main() {
    s01_storageDuration();
    s02_mallocFree();
    s03_newDelete();
    s04_allocationFailure();
    s05_arrayNew();
    s06_twoDimensional();
    s07_modern();
}
