// Rešenje završne vežbe dela 8: pipeline merenja sa condition_variable.

#include <condition_variable>
#include <deque>
#include <future>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

// ---------------------------------------------------------------- korak 1
// Red koji više niti bezbedno puni i prazni. primi() ČEKA dok nešto ne
// stigne ili dok se red ne zatvori -- bez vrćenja u petlji.
template <typename T>
class BezbedanRed {
public:
    void posalji(T x) {
        {
            std::lock_guard<std::mutex> g(m_);
            if (zatvoren_) throw std::logic_error("slanje u zatvoren red");
            q_.push_back(std::move(x));
        }
        cv_.notify_one();                            // probudi jednog koji čeka; van zaključavanja
    }

    // Prazan optional: red je zatvoren I prazan -- nema više posla.
    std::optional<T> primi() {
        std::unique_lock<std::mutex> l(m_);          // wait traži unique_lock (otključa dok spava)
        cv_.wait(l, [this] { return !q_.empty() || zatvoren_; });   // predikat: i lažna buđenja
        if (q_.empty()) return std::nullopt;
        T x = std::move(q_.front());
        q_.pop_front();
        return x;
    }

    void zatvori() {
        {
            std::lock_guard<std::mutex> g(m_);
            zatvoren_ = true;
        }
        cv_.notify_all();                            // SVI koji čekaju treba da vide kraj
    }

private:
    std::mutex m_;
    std::condition_variable cv_;
    std::deque<T> q_;
    bool zatvoren_ = false;
};

struct Merenje {
    int senzor;
    long vrednost;
};

struct Zbir {
    int n = 0;
    long suma = 0;
};

// ---------------------------------------------------------------- korak 2
// Proizvođač šalje n merenja; senzor koji "otkaže" posle k merenja baci izuzetak.
int proizvodjac(BezbedanRed<Merenje>& red, int senzor, int n, int otkazPosle = -1) {
    for (int i = 0; i < n; ++i) {
        if (i == otkazPosle) throw std::runtime_error("senzor " + std::to_string(senzor) + ": nema odgovora");
        red.posalji({senzor, senzor * 100L + i});
    }
    return n;
}

// Potrošač prima dok red ne bude zatvoren i prazan; svaki ima SVOJ zbir, bez deljenja.
std::map<int, Zbir> potrosac(BezbedanRed<Merenje>& red) {
    std::map<int, Zbir> zbir;
    while (auto m = red.primi()) {
        Zbir& z = zbir[m->senzor];
        ++z.n;
        z.suma += m->vrednost;
    }
    return zbir;
}

void spoji(std::map<int, Zbir>& ukupno, const std::map<int, Zbir>& deo) {
    for (const auto& [senzor, z] : deo) {
        ukupno[senzor].n += z.n;
        ukupno[senzor].suma += z.suma;
    }
}

// ---------------------------------------------------------------- korak 3
// RAII: red se zatvara i kad izuzetak preskoči ostatak funkcije -- inače bi
// potrošači čekali zauvek, a future iz async-a bi u destruktoru čekao njih.
class ZatvoriNaKraju {
public:
    explicit ZatvoriNaKraju(BezbedanRed<Merenje>& r) : red_(r) {}
    ~ZatvoriNaKraju() { red_.zatvori(); }
    ZatvoriNaKraju(const ZatvoriNaKraju&) = delete;
    ZatvoriNaKraju& operator=(const ZatvoriNaKraju&) = delete;

private:
    BezbedanRed<Merenje>& red_;
};

struct Rezultat {
    std::map<int, Zbir> poSenzoru;
    std::vector<std::string> greske;
};

Rezultat pokreni(int potrosaca, const std::vector<std::pair<int, int>>& senzori, int otkazujeSenzor, int otkazPosle) {
    BezbedanRed<Merenje> red;
    std::vector<std::future<std::map<int, Zbir>>> p;
    for (int i = 0; i < potrosaca; ++i) p.push_back(std::async(std::launch::async, potrosac, std::ref(red)));

    Rezultat r;
    {
        ZatvoriNaKraju zatvaranje(red);
        std::vector<std::future<int>> izvori;
        for (const auto& [senzor, n] : senzori)
            izvori.push_back(std::async(std::launch::async, proizvodjac, std::ref(red), senzor, n,
                                        senzor == otkazujeSenzor ? otkazPosle : -1));
        for (auto& f : izvori) {
            try {
                f.get();                             // izuzetak iz proizvođača stiže ovde
            } catch (const std::exception& e) {
                r.greske.push_back(e.what());
            }
        }
    }                                                // svi proizvođači gotovi: zatvori red
    for (auto& f : p) spoji(r.poSenzoru, f.get());
    return r;
}

void ispisi(const Rezultat& r) {
    int ukupno = 0;
    for (const auto& [senzor, z] : r.poSenzoru) {
        std::cout << "  senzor " << senzor << ": " << z.n << " merenja, suma " << z.suma << '\n';
        ukupno += z.n;
    }
    std::cout << "  ukupno " << ukupno << " merenja";
    for (const auto& g : r.greske) std::cout << "; greška: " << g;
    std::cout << '\n';
}

int main() {
    std::cout << std::boolalpha << "== korak 1: red u jednoj niti\n";
    BezbedanRed<Merenje> red;
    red.posalji({1, 10});
    red.posalji({2, 20});
    red.zatvori();
    const auto a = red.primi();
    const auto b = red.primi();
    const auto c = red.primi();
    std::cout << "primljeno " << a->vrednost << ", " << b->vrednost << ", posle zatvaranja prazan: " << !c.has_value()
              << '\n';
    try {
        red.posalji({3, 30});
    } catch (const std::logic_error& e) {
        std::cout << "slanje posle zatvaranja: " << e.what() << '\n';
    }

    std::cout << "== korak 2: jedan proizvođač, jedan potrošač (std::thread)\n";
    BezbedanRed<Merenje> r2;
    std::map<int, Zbir> zbir2;
    std::thread potr([&] { zbir2 = potrosac(r2); });
    std::thread proiz([&] {
        proizvodjac(r2, 7, 1000);
        r2.zatvori();
    });
    proiz.join();
    potr.join();
    std::cout << "senzor 7: " << zbir2[7].n << " merenja, suma " << zbir2[7].suma << '\n';

    std::cout << "== korak 3: tri proizvođača, dva potrošača (std::async)\n";
    ispisi(pokreni(2, {{1, 500}, {2, 300}, {3, 200}}, 0, -1));

    std::cout << "== korak 4: senzor 2 otkaže posle 50 merenja\n";
    ispisi(pokreni(3, {{1, 500}, {2, 300}, {3, 200}}, 2, 50));
}
