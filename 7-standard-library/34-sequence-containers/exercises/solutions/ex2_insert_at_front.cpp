// Rešenje zadatka ex2_insert_at_front.

#include <deque>
#include <iostream>
#include <vector>

int moves = 0;

struct Message {
    int id;
    explicit Message(int i) : id(i) {}
    Message(const Message& o) : id(o.id) { ++moves; }
    Message(Message&& o) noexcept : id(o.id) { ++moves; }
    Message& operator=(const Message& o) {
        id = o.id;
        ++moves;
        return *this;
    }
    Message& operator=(Message&& o) noexcept {
        id = o.id;
        ++moves;
        return *this;
    }
};

int main() {
    // Ako često umećeš na početak vektora (nije dobro): svako umetanje
    // pomeri sve elemente -- O(n) po umetanju, O(n^2) ukupno.
    // Treba ovako: deque -- dodavanje na oba kraja je O(1), postojeći
    // elementi ostaju gde jesu. emplace_front pravi element na mestu.
    std::deque<Message> queue;
    for (int i = 0; i < 100; ++i) queue.emplace_front(i);
    std::cout << "first: " << queue.front().id << ", last: " << queue.back().id << '\n';
    std::cout << "element moves: " << moves << '\n';
}
