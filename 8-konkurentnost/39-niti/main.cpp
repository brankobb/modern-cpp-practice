#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <string>
#include <thread>
#include <utility>
#include <vector>

// Niti, mutex, lock_guard -- ISPRAVNI slučajevi.
// Sve se kompajlira bez upozorenja i radi bez ASan/UBSan i bez TSan
// prijava (./build.sh ... --tsan), g++ 13 i clang 18, C++17 i C++20.
// Brojevi sekcija prate notes.md. Niti NE pišu na cout: redosled bi
// zavisio od raspoređivača. Rezultate upisuju u promenljive, a main ih
// ispiše posle join().
// POGREŠNI slučajevi:
//   errors/   -- kod koji se NE kompajlira
//   ub/       -- data race, redosled zaključavanja, detach sa referencom
//   runtime/  -- std::terminate: nit bez join, izuzetak iz niti, dupli join
// ./check_cases.sh 8-konkurentnost/39-niti  proverava sve.

// ---------------------------------------------------------------- 1
void sekcija1() {
    std::cout << "\n== 1. prva nit\n";
    std::string poruka;
    std::thread t([&poruka] { poruka = "pozdrav iz druge niti"; });
    t.join();                       // čeka kraj niti; tek POSLE ovoga je čitanje bezbedno
    std::cout << poruka << '\n';
}

// ---------------------------------------------------------------- 2
int rezultatFunkcije = 0;
void funkcija() { rezultatFunkcije = 1; }

struct Funktor {
    int* izlaz;
    void operator()() const { *izlaz = 3; }
};

struct Senzor {
    int vrednost = 0;
    void citaj(int v) { vrednost = v; }
};

void sekcija2() {
    std::cout << "\n== 2. pravljenje niti: funkcija, lambda, funktor, metoda\n";
    int izLambde = 0, izFunktora = 0;
    Senzor s;
    std::thread t1(funkcija);
    std::thread t2([&izLambde] { izLambde = 2; });
    std::thread t3(Funktor{&izFunktora});
    std::thread t4(&Senzor::citaj, &s, 4);      // metoda: pokazivač na objekat, pa argumenti
    std::cout << "joinable pre join: " << t1.joinable() << '\n';
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    std::cout << "joinable posle join: " << t1.joinable() << '\n';
    std::cout << "rezultati: " << rezultatFunkcije << ' ' << izLambde << ' ' << izFunktora << ' ' << s.vrednost
              << '\n';

    // std::thread se ne kopira, samo premešta (errors/e02).
    std::thread a([] {});
    std::thread b = std::move(a);
    std::cout << "posle move: a.joinable() = " << a.joinable() << ", b.joinable() = " << b.joinable() << '\n';
    b.join();

    std::vector<std::thread> niti;
    std::vector<int> kvadrati(4);
    for (int i = 0; i < 4; ++i)
        niti.emplace_back([&kvadrati, i] { kvadrati[static_cast<std::size_t>(i)] = i * i; });   // svaka nit svoj element
    for (auto& t : niti) t.join();
    std::cout << "vector<thread>, kvadrati:";
    for (int k : kvadrati) std::cout << ' ' << k;
    std::cout << '\n';
}

// ---------------------------------------------------------------- 3
void uvecaj(int& x) { ++x; }
void istaAdresa(const int& x, const int* original, bool& ista) { ista = (&x == original); }
void preuzmi(std::unique_ptr<int> p, int& izlaz) { izlaz = *p; }

void sekcija3() {
    std::cout << "\n== 3. argumenti se KOPIRAJU u nit\n";
    int brojac = 0;
    std::thread t1(uvecaj, std::ref(brojac));   // bez std::ref: errors/e01
    t1.join();
    std::cout << "posle uvecaj(std::ref(brojac)): " << brojac << '\n';

    bool ista = true;
    std::thread t2(istaAdresa, brojac, &brojac, std::ref(ista));   // const int& -- ali na KOPIJU
    t2.join();
    std::cout << "const int& parametar vidi original: " << ista << '\n';

    auto p = std::make_unique<int>(42);
    int preuzeto = 0;
    std::thread t3(preuzmi, std::move(p), std::ref(preuzeto));   // move-only argument: std::move
    t3.join();
    std::cout << "unique_ptr premešten u nit: " << preuzeto << ", p je sada " << (p ? "pun" : "prazan") << '\n';
}

// ---------------------------------------------------------------- 4
// Nit nema povratnu vrednost (šta vrati funkcija, odbaci se). Rezultat
// ide kroz parametar-referencu ili capture; lekcija 40 ima std::future.
void zbirDela(const std::vector<int>& v, std::size_t od, std::size_t doKraja, long& izlaz) {
    izlaz = std::accumulate(v.begin() + static_cast<std::ptrdiff_t>(od),
                            v.begin() + static_cast<std::ptrdiff_t>(doKraja), 0L);
}

void sekcija4() {
    std::cout << "\n== 4. vraćanje rezultata iz niti\n";
    std::vector<int> v(1000);
    std::iota(v.begin(), v.end(), 1);           // 1..1000
    const std::size_t delova = 4, korak = v.size() / delova;
    std::vector<long> delovi(delova);           // svaka nit SVOJ element -- nema deljenja
    std::vector<std::thread> niti;
    for (std::size_t i = 0; i < delova; ++i)
        niti.emplace_back(zbirDela, std::cref(v), i * korak, (i + 1) * korak, std::ref(delovi[i]));
    for (auto& t : niti) t.join();
    std::cout << "delovi:";
    for (long d : delovi) std::cout << ' ' << d;
    std::cout << ", ukupno " << std::accumulate(delovi.begin(), delovi.end(), 0L) << '\n';
}

// ---------------------------------------------------------------- 5
void sekcija5() {
    std::cout << "\n== 5. std::mutex\n";
    long brojac = 0;
    std::mutex m;
    auto posao = [&] {
        for (int i = 0; i < 10000; ++i) {
            m.lock();
            ++brojac;                            // kritična sekcija; bez mutex-a: ub/u01
            m.unlock();
        }
    };
    std::vector<std::thread> niti;
    for (int i = 0; i < 4; ++i) niti.emplace_back(posao);
    for (auto& t : niti) t.join();
    std::cout << "4 niti x 10000: " << brojac << '\n';

    bool uspeo = true;
    m.lock();
    std::thread t([&] {
        uspeo = m.try_lock();                    // ne čeka: false ako je zaključan
        if (uspeo) m.unlock();
    });
    t.join();
    m.unlock();
    std::cout << "try_lock dok ga drži main: " << uspeo << '\n';
}

// ---------------------------------------------------------------- 6
struct Racun {
    std::mutex m;
    int stanje;
    explicit Racun(int s) : stanje(s) {}
};

void prenesi(Racun& sa, Racun& na, int iznos) {
    std::scoped_lock zakljucaj(sa.m, na.m);      // C++17: oba odjednom, bez deadlock-a (ub/u02)
    sa.stanje -= iznos;
    na.stanje += iznos;
}

void sekcija6() {
    std::cout << "\n== 6. lock_guard i scoped_lock (RAII)\n";
    std::vector<std::string> dnevnik;
    std::mutex m;
    auto zapisi = [&](int id) {
        std::lock_guard<std::mutex> zakljucano(m);   // unlock u destruktoru, i kad se baci izuzetak
        dnevnik.push_back("nit " + std::to_string(id));
    };
    std::vector<std::thread> niti;
    for (int i = 0; i < 3; ++i) niti.emplace_back(zapisi, i);
    for (auto& t : niti) t.join();
    std::sort(dnevnik.begin(), dnevnik.end());   // redosled upisa nije određen
    std::cout << "dnevnik (sortiran):";
    for (const auto& d : dnevnik) std::cout << " [" << d << ']';
    std::cout << '\n';

    Racun a(100), b(100);
    std::thread t1([&] { for (int i = 0; i < 1000; ++i) prenesi(a, b, 1); });
    std::thread t2([&] { for (int i = 0; i < 1000; ++i) prenesi(b, a, 1); });   // suprotan redosled
    t1.join();
    t2.join();
    std::cout << "posle 2 x 1000 prenosa u oba smera: " << a.stanje << ' ' << b.stanje << '\n';

    std::unique_lock<std::mutex> ul(m);          // kao lock_guard, ali ume i unlock/lock
    ul.unlock();
    std::cout << "unique_lock posle unlock: owns_lock = " << ul.owns_lock() << '\n';
}

// ---------------------------------------------------------------- 7
void sekcija7() {
    std::cout << "\n== 7. std::thread metode i std::this_thread\n";
    std::thread::id idNiti;
    std::thread t([&idNiti] { idNiti = std::this_thread::get_id(); });
    std::thread::id izSpolja = t.get_id();
    t.join();
    std::cout << "get_id iznutra == get_id spolja: " << (idNiti == izSpolja) << '\n';
    std::cout << "id niti != id main-a: " << (idNiti != std::this_thread::get_id()) << '\n';
    std::cout << "posle join: t.get_id() == std::thread::id{}: " << (t.get_id() == std::thread::id{}) << '\n';

    auto pocetak = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    auto proslo = std::chrono::steady_clock::now() - pocetak;
    std::cout << "sleep_for(20ms) spavao bar 20ms: " << (proslo >= std::chrono::milliseconds(20)) << '\n';
    std::this_thread::yield();                   // "pusti druge" -- samo savet raspoređivaču

    std::thread x([] {}), y;
    std::swap(x, y);
    std::cout << "posle swap: x.joinable() = " << x.joinable() << ", y.joinable() = " << y.joinable() << '\n';
    y.join();
}

int main() {
    std::cout << std::boolalpha;
    sekcija1();
    sekcija2();
    sekcija3();
    sekcija4();
    sekcija5();
    sekcija6();
    sekcija7();
}
