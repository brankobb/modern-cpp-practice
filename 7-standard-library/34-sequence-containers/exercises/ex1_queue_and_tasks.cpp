// KIND: usage
//
// Zadatak 1 -- deque kao red, list sa splice, array kao brojač
// (sekcije 2, 4, 5)
//   ./build.sh 7-standard-library/34-sequence-containers/exercises/ex1_queue_and_tasks.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_queue_and_tasks.cpp
//
// Korak 1: class MessageQueue nad std::deque<std::string>:
//   void receive(std::string p) -- na kraj; void receiveUrgent(std::string p) --
//   na POČETAK; bool process(std::string& out) -- skine prvu poruku
//   (false ako je red prazan). Zašto deque, a ne vector?
// Korak 2: lista zadataka std::list<std::string>. void moveToFront(
//   std::list<std::string>& l, const std::string& ime) -- nađi zadatak
//   (std::find) i PREMESTI ga na početak sa l.splice(l.begin(), l, it):
//   O(1), bez kopiranja stringa, a ostali iteratori ostaju važeći.
// Korak 3: std::array<int, 7> za broj poruka po danu u nedelji (0 =
//   ponedeljak). Nađi najprometniji dan: std::max_element i
//   std::distance za indeks.

#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <list>
#include <string>

// TODO korak 1 i 2

int main() {
    // Korak 1 -- otkomentariši:
    // MessageQueue queue;
    // queue.receive("temp 21");
    // queue.receive("temp 22");
    // queue.receiveUrgent("ALARM pressure");
    // std::string p;
    // while (queue.process(p)) std::cout << "processed: " << p << '\n';

    // Korak 2 -- otkomentariši:
    // std::list<std::string> tasks{"calibration", "log", "backup", "update"};
    // moveToFront(tasks, "backup");
    // std::cout << "tasks:";
    // for (const auto& z : tasks) std::cout << ' ' << z;
    // std::cout << '\n';

    // Korak 3 -- otkomentariši:
    // std::array<int, 7> perDay{12, 30, 7, 30, 45, 3, 0};
    // auto it = std::max_element(perDay.begin(), perDay.end());
    // std::cout << "busiest day: " << std::distance(perDay.begin(), it) << " (" << *it << " messages)\n";
}

/* EXPECTED OUTPUT
processed: ALARM pressure
processed: temp 21
processed: temp 22
tasks: backup calibration log update
busiest day: 4 (45 messages)
*/
