// STD: c++17
// EXPECT-GCC: cannot have cv-qualifier
// EXPECT-CLANG: static member function cannot have 'const' qualifier
// POGREŠNO: static funkcija člana označena sa const.
// Zašto: const na funkciji člana znači "this je const Counter*". static
//   funkcija nema this, pa const nema na šta da se odnosi.
// Ispravno: static int count(); -- bez const.
class Counter {
public:
    static int count() const { return total_; }

private:
    inline static int total_ = 0;
};

int main() {
    return Counter::count();
}
