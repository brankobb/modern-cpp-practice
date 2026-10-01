# Roadmap: od C programera do modernog C++-a

Odvojen put kroz C++, korak po korak. Svaki korak ima:

- **šta učiš** — teme, svaka sa kratkim primerom;
- **šta moraš da razumeš** — ne samo da znaš sintaksu, nego zašto je tako;
- **zadatak** koji pokriva celu temu — kostur koji se kompajlira, tvoj kod,
  pa poređenje sa rešenjem;
- **proveru** pre sledećeg koraka.

Ovaj folder je nezavisan od lekcija 01–41 u ostatku repozitorijuma, ali se
na njih poziva ("Dublje: ...") kad neka tema zaslužuje više od jedne strane.

| Korak | Tema | Folder | Status |
|---|---|---|---|
| 1 | Tranzicija sa C na C++ (temelj): RAII, reference, `const`, `constexpr`, `enum class`, inicijalizacija | [`step-1-c-to-cpp/`](step-1-c-to-cpp/notes.md) | ✅ spreman |
| 2 | Objektni C++ i životni ciklus objekta: konstruktori, Rule of 0/3/5, `operator=`, nasleđivanje, `explicit` | [`step-2-object-lifecycle/`](step-2-object-lifecycle/notes.md) | ✅ spreman |
| 3 | Moderni C++: move semantika, pametni pokazivači, lambde, `constexpr` | — | sledeći |

## Kako raditi

1. Pročitaj `notes.md` koraka i pokreni `demos.cpp`. Za svaku sekciju
   **prvo predvidi** izlaz, pa tek onda pogledaj.
2. Otvori `task/...cpp`. Fajl se kompajlira i nerešen. Piši kod redom po
   koracima iz komentara na vrhu i otkomentarišuj delove `main()`-a.
3. Tvoj izlaz mora da bude **tačno** blok `EXPECTED OUTPUT` na dnu fajla.
4. Tek onda otvori `solutions/` i uporedi **pristup**, ne samo izlaz.
5. Zapiši šta te iznenadilo u "Zapažanja posle vežbe" na kraju `notes.md`.

Build (isti `build.sh` kao za lekcije: ASan + UBSan, `-Wall -Wextra -pedantic-errors`):

```
./build.sh roadmap/step-1-c-to-cpp/demos.cpp
./build.sh roadmap/step-1-c-to-cpp/task/raii_file.cpp
./build.sh roadmap/step-2-object-lifecycle/demos.cpp
./build.sh roadmap/step-2-object-lifecycle/task/buffer.cpp
```

Makroi za demonstraciju greške (`-DTRY_COPY`, `-DNAIVE_ASSIGN`,
`-DNO_VIRTUAL`, `-DREORDER`, `-DTRY_IMPLICIT`) su opisani na vrhu fajla u
kom se koriste. Svaki fajl je proveren sa g++ 13 i clang 18, u C++17 i C++20.

## Pre Koraka 3

Odgovori na pitanja u [`questions.md`](questions.md) (pišeš u tom fajlu),
pa uporedi sa [`answers-reference.md`](answers-reference.md). Ako na neko
pitanje ne znaš da odgovoriš bez razmišljanja — vrati se na tu temu.

U Koraku 3 tvoj `File` iz Koraka 1 dobija move, a `Buffer` iz Koraka 2
postaje osnova za priču o `noexcept`, `std::vector` realokaciji i
pametnim pokazivačima. Tada se vidi zašto sve iz Koraka 2 ima smisla.
