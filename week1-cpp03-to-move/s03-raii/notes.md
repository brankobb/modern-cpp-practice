# Sesija 3 — RAII i exception safety

Izvori: Effective C++ (3. izd.) stavke 5–14, 29;
Arthur O'Dwyer, *Back to Basics: RAII and the Rule of Zero* (CppCon 2019)

- stack unwinding pri throw — koji destruktori se pozivaju i kojim redom
- basic / strong / nothrow garancija — razlika i primeri
- copy-and-swap idiom — zašto daje strong garanciju "besplatno"
- zašto destruktor ne sme da baca izuzetak (šta se dešava ako baci tokom
  unwinding-a već aktivnog izuzetka -> std::terminate)

## Zapažanja posle vežbe
