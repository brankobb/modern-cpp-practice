#!/usr/bin/env python3
"""Proverava unakrsne reference u lekcijama (notes.md i .cpp fajlovi).

Šta se proverava:
  lekcija NN            -> postoji <deo>/NN-* (npr. 3-zivotni-vek-i-resursi/21-raii)
  lekcije NN, MM i KK   -> postoji svaka; raspon NN–MM: svaka između
  errors/eNN, ub/uNN,
  runtime/rNN           -> postoji fajl sa tim prefiksom u ciljnoj lekciji
  sekcija N, sekcije    -> ciljni notes.md ima naslov "# N."
  N, M, ... / N-M / N i M
  exercises/zN          -> postoji zadatak sa tim prefiksom
  1-osnove.../fajl.cpp  -> fajl postoji (putanja od korena repozitorijuma)
  exercises/.../x.cpp   -> fajl postoji u ciljnoj lekciji

Ciljna lekcija je najbliža PRETHODNA oznaka lekcije u istoj rečenici
("lekcija 11, ub/u02", "lekcija 32, sekcija 5", "lekcija 01 (...), 03 (...)",
red tabele "| 03, sekcija 11 |"); ako je nema, to je lekcija u kojoj je
fajl. "main.cpp, sekcija N" se proverava prema redovima "// ----- N" u
main.cpp, a ne prema notes.md.

Proverava sve lekcije i README.md.
Usage: ./check_refs.py            (izlaz 1 ako ima pokvarenih referenci)
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))

LESSON_RE = re.compile(
    r"(?i:lekcij[a-z]*) (\d{2}(?:\s*(?:,|–|-|\bi\b)\s*\d{2}(?!\d))*)(?!\d)"  # lekcija 03, lekcije 23, 25 / 14–17
    r"|\|\s*(\d{2}),"                         # tabela: | 03, sekcija 11 |
)
FILE_RE = re.compile(r"\b(main(?:_cpp20)?\.cpp)\b")
CASE_RE = re.compile(r"\b(errors|ub|runtime)/([eur]\d{2})\b")
EXERCISE_RE = re.compile(r"\bexercises/(z\d)\b")
# Putanje fajlova: od korena repozitorijuma (./build.sh week0-.../main.cpp)
# ili od lekcije (exercises/solutions/z1_ime.cpp).
ROOT_PATH_RE = re.compile(r"(?<![\w/.])([1-9]-[\w-]+/[\w./-]+\.(?:cpp|h|md))")
LESSON_PATH_RE = re.compile(r"(?<![\w/.])(exercises/(?:solutions/)?\w+\.cpp)")
SECTION_RE = re.compile(r"\b(?i:sekcij[aeiu]) (\d+(?:\s*(?:,|-|–|\bi\b)\s*\d+)*)")
# Kraj rečenice: tačka pa razmak pa veliko slovo (reference ne prelaze rečenicu).
SENTENCE_END = re.compile(r"[.!?]\s+(?=[A-ZČĆŠĐŽ])")


def lesson_dirs():
    """Mapira broj lekcije ('03', '21') na folder lekcije (<deo>/NN-ime)."""
    out = {}
    for part in sorted(os.listdir(ROOT)):
        pdir = os.path.join(ROOT, part)
        if not (os.path.isdir(pdir) and re.match(r"\d-", part)):
            continue
        for name in sorted(os.listdir(pdir)):
            path = os.path.join(pdir, name)
            if not os.path.isdir(path):
                continue
            m = re.match(r"(\d{2})-", name)
            if m:
                out[m.group(1)] = path
    return out


def lesson_numbers(text):
    """'23, 25' -> ['23', '25']; '14–17' -> ['14', '15', '16', '17']."""
    nums = []
    for part in re.split(r"\s*(?:,|\bi\b)\s*", text):
        r = re.match(r"(\d{2})\s*[-–]\s*(\d{2})$", part)
        if r:
            nums.extend(f"{n:02d}" for n in range(int(r.group(1)), int(r.group(2)) + 1))
        elif re.fullmatch(r"\d{2}", part):
            nums.append(part)
    return nums


def headings(notes_path):
    """Brojevi sekcija "# N." iz notes.md."""
    if not os.path.exists(notes_path):
        return set()
    nums = set()
    with open(notes_path, encoding="utf-8") as f:
        in_code = False
        for line in f:
            if line.startswith("```"):
                in_code = not in_code
            if in_code:
                continue
            m = re.match(r"#{1,2} (\d+)\.", line)
            if m:
                nums.add(int(m.group(1)))
    return nums


def section_numbers(text):
    nums = []
    for part in re.split(r"\s*(?:,|\bi\b)\s*", text):
        r = re.match(r"(\d+)\s*[-–]\s*(\d+)$", part)
        if r:
            nums.extend(range(int(r.group(1)), int(r.group(2)) + 1))
        elif part.isdigit():
            nums.append(int(part))
    return nums


def paragraphs(lines, is_md):
    """Delovi teksta u kojima se reference traže zajedno.

    .md: pasusi (između praznih redova); svaka stavka liste i svaki red
    tabele su poseban deo.
    .cpp: uzastopni komentari (bez "//"), i svaki red koda posebno.
    Vraća (tekst, funkcija pozicija -> broj reda).
    """
    groups = []
    cur = []
    for i, raw in enumerate(lines, 1):
        line = raw.rstrip("\n")
        if is_md:
            if not line.strip() or line.startswith("|"):
                if cur:
                    groups.append(cur)
                cur = []
                if line.startswith("|"):
                    groups.append([(i, line)])
                continue
            # Nova stavka liste počinje novi deo teksta.
            if re.match(r"\s*(?:[-*]|\d+\.) ", line) and cur:
                groups.append(cur)
                cur = []
            cur.append((i, line))
        else:
            m = re.match(r"\s*//(.*)", line)
            if m:
                # "Rešenje: exercises/..." u zaglavlju zadatka se odnosi na
                # lekciju fajla, ne na lekciju pomenutu u naslovu iznad.
                if m.group(1).strip().startswith("Rešenje:") and cur:
                    groups.append(cur)
                    cur = []
                cur.append((i, m.group(1)))
            else:
                if cur:
                    groups.append(cur)
                cur = []
                if "//" in line:
                    groups.append([(i, line[line.index("//") + 2:])])
    if cur:
        groups.append(cur)

    out = []
    for g in groups:
        text = ""
        starts = []
        for i, l in g:
            starts.append((len(text), i))
            text += l.strip() + " "

        def line_of(pos, starts=starts):
            n = starts[0][1]
            for off, i in starts:
                if off <= pos:
                    n = i
            return n

        out.append((text, line_of))
    return out


def check_paragraph(text, line_of, rel, ldir, lessons, head_cache, problems, stats):
    lesson_marks = []
    for m in LESSON_RE.finditer(text):
        tags = lesson_numbers(m.group(1)) if m.group(1) else [m.group(2)]
        lesson_marks.append((m.start(), tags[-1]))   # dalje reference se odnose na poslednju
        for tag in tags:
            stats["lekcija"] += 1
            if tag not in lessons:
                problems.append(f"{rel}:{line_of(m.start())}: nema lekcije {tag} ('{m.group(0)}')")
    file_marks = [(m.start(), m.group(1)) for m in FILE_RE.finditer(text)]
    ends = [m.start() for m in SENTENCE_END.finditer(text)]

    def same_sentence(a, b):
        return not any(a < e < b for e in ends)

    # Nabrajanje posle "lekcija": "lekcija 01 (...), 03 (...), 05 (...)".
    for m in re.finditer(r"(?<![\w.])(\d{2}) \(", text):
        if any(p < m.start() and same_sentence(p, m.start()) for p, _ in lesson_marks):
            lesson_marks.append((m.start(), m.group(1)))
    lesson_marks.sort()

    def target(pos):
        """(folder lekcije, fajl ili None) za referencu na poziciji pos."""
        prev = [(p, t) for p, t in lesson_marks if p < pos and same_sentence(p, pos)]
        tdir = lessons.get(prev[-1][1]) if prev else ldir
        start = prev[-1][0] if prev else -1
        files = [(p, f) for p, f in file_marks if start < p < pos and same_sentence(p, pos)]
        return tdir, (files[-1][1] if files else None)

    for m in CASE_RE.finditer(text):
        stats["errors/ub"] += 1
        tdir, _ = target(m.start())
        if tdir is None:
            continue
        sub = os.path.join(tdir, m.group(1))
        names = os.listdir(sub) if os.path.isdir(sub) else []
        if not any(n.startswith(m.group(2) + "_") for n in names):
            problems.append(f"{rel}:{line_of(m.start())}: nema {m.group(1)}/{m.group(2)} u "
                            f"{os.path.relpath(tdir, ROOT)}")

    for m in EXERCISE_RE.finditer(text):
        stats["exercises"] += 1
        tdir, _ = target(m.start())
        if tdir is None:
            continue
        sub = os.path.join(tdir, "exercises")
        names = os.listdir(sub) if os.path.isdir(sub) else []
        if not any(n.startswith(m.group(1) + "_") for n in names):
            problems.append(f"{rel}:{line_of(m.start())}: nema exercises/{m.group(1)}")

    for m in ROOT_PATH_RE.finditer(text):
        stats["putanja"] += 1
        if not os.path.exists(os.path.join(ROOT, m.group(1))):
            problems.append(f"{rel}:{line_of(m.start())}: nema fajla {m.group(1)}")

    for m in LESSON_PATH_RE.finditer(text):
        tdir, _ = target(m.start())
        if tdir is None:
            continue
        stats["putanja"] += 1
        if not os.path.exists(os.path.join(tdir, m.group(1))):
            problems.append(f"{rel}:{line_of(m.start())}: nema fajla {m.group(1)} u "
                            f"{os.path.relpath(tdir, ROOT)}")

    for m in SECTION_RE.finditer(text):
        tdir, fname = target(m.start())
        if tdir is None:
            continue
        src = os.path.join(tdir, fname or "notes.md")
        if src not in head_cache:
            head_cache[src] = code_sections(src) if fname else headings(src)
        for n in section_numbers(m.group(1)):
            stats["sekcija"] += 1
            if n not in head_cache[src]:
                problems.append(f"{rel}:{line_of(m.start())}: nema sekcije {n} u "
                                f"{os.path.relpath(src, ROOT)} ('{m.group(0).strip()}')")


def code_sections(path):
    """Sekcije u main.cpp: red "// ------...------ N"."""
    if not os.path.exists(path):
        return set()
    nums = set()
    with open(path, encoding="utf-8") as f:
        for line in f:
            m = re.match(r"// -{8,} (\d+)\s*$", line)
            if m:
                nums.add(int(m.group(1)))
    return nums


def main():
    lessons = lesson_dirs()
    head_cache = {}
    problems = []
    stats = {"lekcija": 0, "errors/ub": 0, "exercises": 0, "sekcija": 0, "putanja": 0}

    for key, ldir in lessons.items():
        for dirpath, _, files in os.walk(ldir):
            for fn in sorted(files):
                if not (fn.endswith(".md") or fn.endswith(".cpp") or fn.endswith(".h")):
                    continue
                path = os.path.join(dirpath, fn)
                rel = os.path.relpath(path, ROOT)
                with open(path, encoding="utf-8") as f:
                    lines = f.readlines()
                for text, line_of in paragraphs(lines, fn.endswith(".md")):
                    check_paragraph(text, line_of, rel, ldir, lessons, head_cache, problems, stats)

    # README: reference bez oznake lekcije nemaju cilj, pa se preskaču.
    readme = os.path.join(ROOT, "README.md")
    if os.path.exists(readme):
        with open(readme, encoding="utf-8") as f:
            lines = f.readlines()
        for text, line_of in paragraphs(lines, True):
            check_paragraph(text, line_of, "README.md", None, lessons, head_cache, problems, stats)

    for p in problems:
        print(p)
    print("provereno: " + ", ".join(f"{k} {v}" for k, v in stats.items()))
    print(f"{len(problems)} problema" if problems else "sve reference su ispravne")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
