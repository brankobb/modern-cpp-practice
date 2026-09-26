// Rešenje zadatka ex3_strogo_manje.

#include <algorithm>
#include <iostream>
#include <set>
#include <tuple>
#include <vector>

struct Verzija {
    int major;
    int minor;
};

// Ako koristiš <= u operator< (nije dobro): a < a je true, pa set ne
// prepoznaje jednake elemente, a std::sort čita van niza (UB).
// Treba ovako: std::tie pravi tuple referenci, a tuple-ov operator< je
// leksikografski i strog -- ispravan i za tri, četiri... polja.
bool operator<(const Verzija& a, const Verzija& b) {
    return std::tie(a.major, a.minor) < std::tie(b.major, b.minor);
}

int main() {
    std::set<Verzija> s{{1, 2}, {1, 2}, {2, 0}};
    std::cout << "velicina: " << s.size() << ", count({1, 2}): " << s.count({1, 2}) << '\n';
    std::vector<Verzija> v(40, Verzija{1, 1});
    v.push_back({0, 9});
    std::sort(v.begin(), v.end());
    std::cout << "prvi: " << v.front().major << '.' << v.front().minor << '\n';
}
