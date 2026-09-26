// STD: c++17
// EXPECT-GCC: call of overloaded 'play()' is ambiguous
// EXPECT-CLANG: call to 'play' is ambiguous
// POGREŠNO: dve using-direktive uvedu isto ime iz dva namespace-a.
// Zašto: direktiva samo učini imena vidljivim. Obe funkcije play() su sada
//   jednako dobri kandidati, a nijedna nije bolja. Sam "using namespace" nije
//   greška; greška se pojavi tek na mestu upotrebe, često mnogo kasnije,
//   kada neko doda novu funkciju u jedan od namespace-a.
// Ispravno: kvalifikuj poziv (audio::play()), ili uvedi samo jedno ime
//   using-deklaracijom (using audio::play;).
namespace audio { void play() {} }
namespace video { void play() {} }

using namespace audio;
using namespace video;

int main() {
    play();
}
