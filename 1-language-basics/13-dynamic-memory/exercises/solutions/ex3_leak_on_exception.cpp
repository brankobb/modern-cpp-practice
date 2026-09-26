// Rešenje zadatka ex3_leak_on_exception.

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

int alive = 0;

struct Channel {
    explicit Channel(int channelId) : id(channelId) {
        if (channelId == 3) throw std::runtime_error("channel 3 does not exist");
        ++alive;
    }
    ~Channel() { --alive; }
    Channel(const Channel& o) : id(o.id) { ++alive; }
    Channel& operator=(const Channel&) = default;
    int id;
};

// Ako radiš "Channel** c = new Channel*[n]; c[i] = new Channel(i);" (nije
// dobro): izuzetak u sredini petlje ostavi niz i sve već napravljene
// kanale bez vlasnika -- curenje, a destruktori se ne pozovu (ako Channel
// drži hardverski resurs, i on ostane zauzet).
// Treba ovako: svaki resurs odmah dobije vlasnika (unique_ptr), a vlasnici
// su u vektoru. Izuzetak -> vektor se uništi -> sve se oslobodi.
std::vector<std::unique_ptr<Channel>> openAll(int n) {
    std::vector<std::unique_ptr<Channel>> c;
    for (int i = 0; i < n; ++i) c.push_back(std::make_unique<Channel>(i));
    return c;
}

// Možeš i ovako, kad objekti ne moraju imati stabilne adrese: sami
// objekti u vektoru. reserve -> nema premeštanja pri rastu.
std::vector<Channel> openAllByValue(int n) {
    std::vector<Channel> c;
    c.reserve(static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i) c.emplace_back(i);
    return c;
}

int main() {
    try {
        auto c = openAll(5);
    } catch (const std::exception& e) {
        std::cout << "error: " << e.what() << ", channels alive: " << alive << '\n';
    }
    auto three = openAll(3);
    std::cout << "opened: " << three.size() << ", alive: " << alive << '\n';

    try {
        auto c = openAllByValue(5);
    } catch (const std::exception& e) {
        std::cout << "by value: " << e.what() << ", channels alive: " << alive - 3 << '\n';
    }
}
