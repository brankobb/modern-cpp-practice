// EXPECT-UB: heap-use-after-free
// POGREŠNO: sirov pokazivač iz get() koristi se posle reset() vlasnika.
// Zašto: get() daje posmatrača (R.3), a posmatrač ne produžava život.
//   Kad vlasnik obriše objekat, observer pokazuje na oslobođenu memoriju.
//   (Napomena: ASan vidi samo instrumentisan kod. Da je Session std::string
//   a čitanje observer->size(), kod iz libstdc++ ne bi bio proveren i
//   greška bi prošla neprijavljena -- test.)
// Ispravno: posmatrač sme da živi kraće od vlasnika; ako to nije sigurno,
//   shared_ptr za vlasnika i weak_ptr za posmatrača (lock() pre upotrebe).
#include <cstdio>
#include <memory>

struct Session {
    int id = 7;
};

int main() {
    auto owner = std::make_unique<Session>();
    Session* observer = owner.get();
    owner.reset();
    std::printf("%d\n", observer->id);
}
