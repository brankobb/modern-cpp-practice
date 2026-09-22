#include <iostream>
#include <vector>

void autoDropsConstRef() {
    int x = 5;
    const int& cref = x;
    auto y = cref;       // y je int (NE const int&) -- moze da se menja
    y = 99;
    std::cout << "x=" << x << " y=" << y << " (x nepromenjen jer je y kopija)\n";
}

struct Item {
    int value;
    void doubleIt() { value *= 2; }
};

void rangeForByValueTrap() {
    std::vector<Item> items = {{1}, {2}, {3}};

    for (auto item : items) { // BUG (namerno): ovo je kopija!
        item.doubleIt();
    }
    std::cout << "posle 'auto item' petlje: ";
    for (const auto& item : items) std::cout << item.value << " ";
    std::cout << "(nepromenjeno -- radili smo na kopijama)\n";

    for (auto& item : items) { // ISPRAVNO: referenca menja original
        item.doubleIt();
    }
    std::cout << "posle 'auto& item' petlje: ";
    for (const auto& item : items) std::cout << item.value << " ";
    std::cout << "(sad JESTE promenjeno)\n";
}

int main() {
    autoDropsConstRef();
    rangeForByValueTrap();
}
