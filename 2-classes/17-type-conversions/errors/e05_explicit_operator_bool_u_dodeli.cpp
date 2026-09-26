// STD: c++17
// EXPECT-GCC: cannot convert 'Handle' to 'bool' in initialization
// EXPECT-CLANG: no viable conversion from 'Handle' to 'bool'
// POGREŠNO: bool ok = h; kad je operator bool explicit.
// Zašto: explicit operator bool radi samo u "kontekstu bool-a" (if, while,
//   !, &&, ||, ?:). Tako ne može slučajno da se desi int n = h; ili h + 1,
//   što bi radilo sa običnim operator bool (bool -> int je promocija).
//   To je problem koji je pre C++11 rešavan idiomom "safe bool".
// Ispravno: bool ok = static_cast<bool>(h); ili bool ok(h); ili if (h).
class Handle {
public:
    explicit operator bool() const { return ptr_ != nullptr; }

private:
    int* ptr_ = nullptr;
};

int main() {
    Handle h;
    bool ok = h;
    return ok;
}
