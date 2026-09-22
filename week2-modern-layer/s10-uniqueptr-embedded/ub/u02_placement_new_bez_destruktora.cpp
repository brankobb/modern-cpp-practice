// EXPECT-UB: LeakSanitizer: detected memory leaks
// POGREŠNO: placement new u isti bafer više puta, bez ručnog destruktora.
// Zašto: placement new samo konstruiše; niko ne poziva ~Message() za
//   prethodni objekat. Message drži std::string sa bafera na heap-u, i taj
//   bafer se nikad ne oslobodi (curenje po svakom emplace-u). Za tipove bez
//   resursa (int, POD) ovo "ne smeta", pa se greška primeti tek kad tip
//   dobije član sa destruktorom.
// Ispravno: pre novog objekta m->~Message(); -- ili klasa koja to radi
//   sama (StaticStorage::emplace i destruktor, main.cpp deo 3).
#include <cstdio>
#include <new>
#include <string>

struct Message {
    std::string text = std::string(64, 'm');
};

alignas(Message) unsigned char storage[sizeof(Message)];

int main() {
    for (int i = 0; i < 3; ++i) {
        Message* m = new (storage) Message;
        std::printf("%zu\n", m->text.size());
    }
}
