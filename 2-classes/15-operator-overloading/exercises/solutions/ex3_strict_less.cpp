// Rešenje zadatka ex3_strict_less.

#include <algorithm>
#include <iostream>
#include <set>
#include <tuple>
#include <vector>

struct Version {
    int major;
    int minor;
};

// Ako koristiš <= u operator< (nije dobro): a < a je true, pa set ne
// prepoznaje jednake elemente, a std::sort čita van niza (UB).
// Treba ovako: std::tie pravi tuple referenci, a tuple-ov operator< je
// leksikografski i strog -- ispravan i za tri, četiri... polja.
bool operator<(const Version& a, const Version& b) {
    return std::tie(a.major, a.minor) < std::tie(b.major, b.minor);
}

int main() {
    std::set<Version> s{{1, 2}, {1, 2}, {2, 0}};
    std::cout << "size: " << s.size() << ", count({1, 2}): " << s.count({1, 2}) << '\n';
    std::vector<Version> v(40, Version{1, 1});
    v.push_back({0, 9});
    std::sort(v.begin(), v.end());
    std::cout << "first: " << v.front().major << '.' << v.front().minor << '\n';
}
