// STD: c++17
// EXPECT-GCC: cannot convert 'std::string'
// EXPECT-CLANG: no viable conversion from 'std::string'
// POGREŠNO: log(double) u izvedenoj klasi, a poziv log(std::string).
// Zašto: traženje imena staje u PRVOM scope-u gde nađe ime log. U
//   FileLogger to je log(double), pa se Logger::log(string) i log(int)
//   uopšte ne razmatraju ([class.member.lookup]). Overload ne ide preko
//   granice klase. log(5) bi se tiho kompajlirao kao log(double).
// Ispravno: "using Logger::log;" u FileLogger (main.cpp, sekcija 3), ili
//   drugo ime za novu funkciju.
#include <string>

class Logger {
public:
    virtual ~Logger() = default;
    void log(const std::string&) {}
    void log(int) {}
};

class FileLogger : public Logger {
public:
    void log(double) {}
};

int main() {
    FileLogger f;
    f.log(std::string("x"));
}
