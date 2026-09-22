# 06 — Reference vs Pointer (24)

| | Pointer | Reference |
|---|---|---|
| Može biti null | da | ne (ali može dangle-ovati) |
| Može se rebind-ovati | da | ne |
| Mora se inicijalizovati odmah | ne | da |
| Aritmetika | da (`p+1`) | ne |
| Sintaksa pristupa | `*p`, `p->` | direktno kao objekat |

## Kad koristiti šta
- **referenca** kad parametar MORA postojati (nema "opciono" stanje) i ne
  treba da se menja koji objekat referiše tokom života — ovo je i DEFAULT
  izbor za prosleđivanje objekata funkcijama (izbegava kopiranje)
- **pokazivač** kad ti treba "opciono" (može biti `nullptr`), ili kad menjaš
  NA ŠTA pokazuje tokom vremena (npr. iteracija, linked struktura)
- posebno bitno kod **polimorfizma**: prosleđuj/čuvaj polimorfne objekte
  preko reference ili pokazivača na Base, NIKAD po vrednosti (setiš se
  slicing-a iz week1 s02 — ovo je isti razlog)

## API korišćen u vežbi

- `override` (C++11, kontekstualni keyword — ne menja tip) — eksplicitno
  traži od kompajlera da proveri da ova funkcija STVARNO override-uje
  virtualnu funkciju iz baze; ako se potpisi ne poklapaju (tipfeler,
  pogrešan `const`), GREŠKA pri kompajliranju umesto tihe nepovezane
  funkcije koja se nikad ne bi pozvala polimorfno
- `= default` na destruktoru (`virtual ~Base() = default;`) — traži od
  kompajlera da generiše podrazumevanu implementaciju; i dalje pravi
  VIRTUAL destructor (jer je `virtual` eksplicitno napisano), samo mu je
  telo prazno i generisano

## Zapažanja posle vežbe
