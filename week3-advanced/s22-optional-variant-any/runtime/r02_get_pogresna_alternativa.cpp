// EXPECT-RUN: terminate called after throwing an instance of 'std::bad_variant_access'
// POGREŠNO: get<int> dok variant drži std::string baca
// std::bad_variant_access.
// Ispravno: std::holds_alternative<int>(v) pre get-a, get_if (pokazivač
// ili nullptr), ili std::visit, koji uvek pozove pravu granu.
#include <string>
#include <variant>
int main() {
    std::variant<int, std::string> v = std::string("temp");
    return std::get<int>(v);
}
