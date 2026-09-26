# Završna vežba dela 8 — Pipeline merenja

Više senzora (proizvođači) šalje merenja u zajednički red, a nekoliko
radnika (potrošači) ih uzima i sabira po senzoru. Na kraju se rezultati
radnika spoje u jedan izveštaj. Jedan senzor usput otkaže: njegova greška
treba da stigne do izveštaja, a ostali senzori da se obrade do kraja, bez
zaglavljivanja. Spaja lekcije 39 i 40, a uvodi jednu novu stvar:
`std::condition_variable`, jer red iz kog potrošač **čeka** posao ne može
lepo da se napravi samo od mutex-a.

**Kako raditi:**

```
./build.sh 8-concurrency/capstone/task.cpp            # tvoj kod (ASan + UBSan)
./build.sh 8-concurrency/capstone/task.cpp --tsan     # ThreadSanitizer
./check_exercises.sh 8-concurrency/capstone              # provera: zadatak i rešenje
```

Koraci su u komentaru na vrhu `task.cpp`; `main()` je napisan i
zakomentarisan po koracima. `Reading`, `Sum`, `mergeInto`, `Result` i
`print` su dati. Rešenje je u `solution.cpp`. Prvo pročitaj sekciju
"Nova tema" ispod. Očekuj 2–3 sedenja.

---

## Nova tema: `std::condition_variable`

**Problem.** Potrošač treba da uzme sledeće merenje, a red je trenutno
prazan. Sa samim mutex-om ostaje samo vrćenje u petlji: zaključaj,
pogledaj, otključaj, pa opet. To troši procesor, a ništa ne radi.
`std::condition_variable` (`<condition_variable>`) omogućava da nit
**spava** dok joj druga nit ne javi da se stanje promenilo.

```cpp
std::mutex m;
std::condition_variable cv;
std::deque<int> q;               // stanje: ono što se čeka, pod mutex-om m

// nit koja čeka
std::unique_lock<std::mutex> l(m);
cv.wait(l, [&] { return !q.empty(); });   // spava; budi se kad je predikat tačan
int x = q.front(); q.pop_front();         // mutex je opet zaključan

// nit koja javlja
{ std::lock_guard<std::mutex> g(m); q.push_back(42); }
cv.notify_one();
```

- ✅ **`wait` prima `std::unique_lock`, a ne `lock_guard`.** Dok spava,
  `wait` **otključa** mutex, da bi druga nit mogla da promeni stanje, i
  ponovo ga zaključa pre nego što se vrati. `lock_guard` to ne ume;
  `unique_lock` ume (lekcija 39, sekcija 6).
- ✅ **Uvek `wait` sa predikatom.** `cv.wait(l, pred)` je po standardu isto
  što i `while (!pred()) cv.wait(l);` ([thread.condition.condvar]).
  Predikat rešava dva problema:
  - *lažno buđenje*: standard dozvoljava da se `wait` vrati i kad niko
    nije javio ("spurious wakeup"), pa se stanje posle buđenja mora
    proveriti;
  - *izgubljeno obaveštenje*: `notify` ne ostavlja trag. Ako stigne pre
    nego što je druga nit počela da čeka, izgubljeno je. Predikat se
    proverava **pre** spavanja, pa nit koja kasni odmah vidi da je stanje
    već spremno.

  Provereno (g++ 13): `notify_one()` pa tek onda `wait_for(l, 200ms)` bez
  predikata vrati `cv_status::timeout`. Isti poziv sa predikatom odmah
  vrati `true`. Core Guidelines CP.42: "Don't wait without a condition";
  EMC Item 39 opisuje oba problema.
- ✅ **Stanje se menja pod mutex-om, i kad je samo `bool`.** Inače nit
  koja čeka može da proveri predikat (lažan), onda druga nit promeni
  stanje i javi, a tek posle toga prva zaspi. Obaveštenje se izgubi čak i
  sa predikatom.
- ⚠️ **`notify_one` ili `notify_all`.** `notify_one` budi jednu nit koja
  čeka i dovoljan je kad je stiglo jedno merenje, jer posao može da uzme
  samo jedan. Kad se red **zatvara**, to moraju da vide **svi**
  potrošači, pa ide `notify_all`. Provereno: sa `notify_one` u `close()`
  rešenje se zaglavilo u 3 od 5 pokretanja, a u 2 je prošlo. Greška zavisi
  od rasporeda niti, pa je jedno uspešno pokretanje ne otkriva.
- ✅ **`notify` posle otključavanja.** Radi ispravno i pod mutex-om, ali
  tada probuđena nit može odmah da naleti na još zaključan mutex. Zato
  `send` zatvara zagradu pre `notify_one`. Ovo je optimizacija, a ne
  pitanje ispravnosti.

**Zatvaranje reda.** Potrošač ne može da zna da više neće biti merenja
ako mu to neko ne kaže. Zato red ima stanje "zatvoren". Tada `receive()`
vraća prazan `std::optional`, ali tek kad je red i prazan, da se ništa
poslato ne izgubi (lekcija 37, sekcija 1). Petlja potrošača je onda samo
`while (auto m = queue.receive())`.

## Šta vežba spaja

| Korak | Šta radiš | Lekcija |
|---|---|---|
| 1 | `SafeQueue<T>`: mutex, `condition_variable`, zatvaranje; `optional` kao "nema više" | nova tema; lekcija 39, sekcije 5 i 6; lekcija 37, sekcija 1 |
| 2 | proizvođač i potrošač u `std::thread`; svaki potrošač ima svoj rezultat | lekcija 39, sekcije 2 i 4 |
| 3 | isto preko `std::async`; red kroz `std::ref`; RAII koji zatvara red | lekcija 40, sekcija 1; lekcija 21, sekcija 1 |
| 3–4 | izuzetak proizvođača stiže kroz `future::get()` u izveštaj | lekcija 40, sekcija 6 |

## Na šta da paziš

- ⚠️ **Zaboravljeno zatvaranje = večno čekanje.** Ako se red ne zatvori,
  potrošači spavaju zauvek. Tada i program visi: destruktor `future`-a iz
  `std::async` čeka da se njegova nit završi (lekcija 40, sekcija 4).
  Provereno: kad se `close()` pozove običnim redom posle petlje sa
  `get()`, a izuzetak iz proizvođača izađe iz `run`, program se
  zaglavi u 3 od 3 pokretanja. `catch` u `main` se nikad ne izvrši, jer
  se unwinding zaustavi na destruktoru vektora potrošačkih `future`-a.
  Zato `CloseAtEnd`: destruktor se izvrši i kad izuzetak preskoči
  ostatak bloka (lekcija 21, sekcija 2).
- ⚠️ **`get()` potrošača tek posle bloka.** Potrošački `future`-i nastaju
  **pre** bloka sa proizvođačima, a njihov `get()` ide **posle** njega. Tek
  kad blok završi, `CloseAtEnd` zatvori red, pa potrošači mogu da
  izađu iz petlje. Provereno: kad se petlja sa `mergeInto(..., f.get())`
  premesti u blok, `run` se zaglavi u 3 od 3 pokretanja. `get()`
  čeka potrošača, a potrošač čeka zatvaranje koje dolazi tek posle.
- ⚠️ **Slanje posle zatvaranja baca.** Tiho odbacivanje bi sakrilo grešku
  u redosledu gašenja. Izuzetak je glasan (korak 1).
- ✅ **Bez deljenog rezultata.** Svaki potrošač sabira u svoju mapu, a
  spajanje ide posle `get()`, u jednoj niti. Jedini deljeni objekat je
  red, i on ima svoj mutex. Zato TSan nema šta da prijavi.
- ✅ **Rezultat ne zavisi od rasporeda.** Ne zna se koji potrošač je
  dobio koje merenje, ali zbir po senzoru je uvek isti. Izveštaj ide kroz
  `std::map`, pa je i redosled ispisa isti. Zato izlaz može da se uporedi
  sa blokom EXPECTED OUTPUT.
- ⚠️ U koraku 4 senzor 2 pošalje tačno 50 merenja (0..49), pa baci. Ono
  što je već poslao se obradi: suma 200·50 + (0 + … + 49) = 11225.
- ℹ️ Paralelni algoritmi (lekcija 41) ovde nisu korišćeni. Sa TBB
  pozadinom ThreadSanitizer prijavljuje lažne trke, a ova vežba mora da
  prođe `--tsan` čisto.

## Posle rešenja

1. Red nema gornju granicu. Šta ako senzori šalju brže nego što radnici
   stižu? Dodaj kapacitet: `send` čeka dok ima mesta. Koliko
   `condition_variable` ti treba, i ko koga budi?
2. Šta bi se desilo da `receive()` vrati prazan `optional` čim je red
   zatvoren, a ne tek kad je zatvoren **i prazan**? Koji bi se broj u
   izlazu promenio, i da li uvek za isto?
3. U koraku 2 proizvođač je `std::thread`, a ne `std::async`. Šta bi se
   desilo da on baci (lekcija 39, runtime primeri)? Zašto u koraku 3 to
   nije problem?

## Zapažanja posle vežbe
