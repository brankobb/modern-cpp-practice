# 01 — Primitivni tipovi, I/O, funkcije (14–17)

- `sizeof` po platformi nije garantovan (samo minimalni opsezi) — nikad ne pretpostavljaj tačnu veličinu
- integer promotion: `char`/`short` u aritmetici se promovišu u `int` pre operacije
- signed/unsigned poređenje — miks izaziva implicitnu konverziju u unsigned, često pogrešno (npr. `-1 > 0u`)
- integer overflow kod signed je **UB** (undefined behavior), kod unsigned je **definisan wraparound**
- `cin >> x` kad parsiranje ne uspe ostavlja stream u fail state — sledeća čitanja se ignorišu dok ne uradiš `cin.clear()` + `cin.ignore()`
- funkcije: parametri po vrednosti su DEFAULT (kopija); povratna vrednost takođe kopija/move (videćeš RVO kasnije u week1)

## API korišćen u vežbi

- `sizeof(T)` — kompajlersko vreme, vraća veličinu tipa u bajtovima (`size_t`);
  NIJE garantovano isto na svim platformama za iste C++ tipove (standard
  garantuje samo minimalne opsege, npr. `int` je bar 16 bita)
- `std::numeric_limits<int>::max()` / `min()` (header `<limits>`) — template
  koji daje maksimalnu/minimalnu vrednost tipa PRENOSIVO preko platformi;
  bolje od C makroa `INT_MAX` jer radi za BILO KOJI tip
  (`std::numeric_limits<double>::max()` itd.) i type-safe je
- `std::cin >> x` — extraction operator; pokušava da parsira formatiran ulaz
  u `x`; vraća referencu na stream koja se implicitno konvertuje u `bool`
  (zato `if (!(std::cin >> x))` radi — proverava da li je parsiranje uspelo)
- `std::cin.clear()` — resetuje error flagove stream-a (`failbit`/`badbit`)
  posle neuspešnog parsiranja; bez ovoga SVA sledeća čitanja se tiho
  ignorišu
- `std::cin.ignore(n, delim)` — odbacuje do `n` karaktera iz ulaznog bafera
  ili dok ne naiđe na `delim`, šta god prvo — koristi se da "isprazni"
  ostatak lošeg unosa posle `clear()`
- `std::numeric_limits<std::streamsize>::max()` — ovde znači "ignoriši
  praktično neograničeno mnogo karaktera dok ne naiđeš na newline";
  `streamsize` je tip koji `ignore()` očekuje kao prvi parametar

## Zapažanja posle vežbe
