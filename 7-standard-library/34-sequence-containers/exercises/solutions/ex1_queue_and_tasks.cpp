// Rešenje zadatka ex1_queue_and_tasks.

#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <utility>

// Korak 1: deque -- push_front i pop_front su O(1). Vector bi za svaku
// hitnu poruku i svaku obrađenu pomerao ceo niz (vector nema ni
// push_front ni pop_front, errors/e04).
class MessageQueue {
public:
    void receive(std::string p) { queue_.push_back(std::move(p)); }
    void receiveUrgent(std::string p) { queue_.push_front(std::move(p)); }
    bool process(std::string& out) {
        if (queue_.empty()) return false;
        out = std::move(queue_.front());
        queue_.pop_front();
        return true;
    }

private:
    std::deque<std::string> queue_;
};

// Korak 2: splice samo prevezuje čvor -- string se ne kopira ni ne pomera.
void moveToFront(std::list<std::string>& l, const std::string& name) {
    auto it = std::find(l.begin(), l.end(), name);
    if (it != l.end()) l.splice(l.begin(), l, it);
}

int main() {
    MessageQueue queue;
    queue.receive("temp 21");
    queue.receive("temp 22");
    queue.receiveUrgent("ALARM pressure");
    std::string p;
    while (queue.process(p)) std::cout << "processed: " << p << '\n';

    std::list<std::string> tasks{"calibration", "log", "backup", "update"};
    moveToFront(tasks, "backup");
    std::cout << "tasks:";
    for (const auto& z : tasks) std::cout << ' ' << z;
    std::cout << '\n';

    // Korak 3: max_element vraća iterator na PRVI najveći; distance daje indeks.
    std::array<int, 7> perDay{12, 30, 7, 30, 45, 3, 0};
    auto it = std::max_element(perDay.begin(), perDay.end());
    std::cout << "busiest day: " << std::distance(perDay.begin(), it) << " (" << *it << " messages)\n";
}
