/* Korak 1 -- BONUS: isti posao u C-u, sa "goto cleanup".
 *   gcc -std=c11 -Wall -Wextra -g -fsanitize=address,undefined \
 *       roadmap/step-1-c-to-cpp/solutions/cleanup_goto.c -o /tmp/cleanup && /tmp/cleanup
 *
 * Uporedi sa copy_and_fail() iz raii_file.cpp:
 *   - ovde SVAKI izlaz iz funkcije mora da prođe kroz "cleanup" labelu;
 *     jedan zaboravljeni "return -1;" umesto "goto cleanup;" = curenje;
 *   - redosled oslobađanja pišeš ručno (i možeš da pogrešiš);
 *   - svaki resurs mora da bude inicijalizovan na NULL PRE prvog goto-a,
 *     inače cleanup zatvara smeće;
 *   - greška se prenosi kroz povratnu vrednost, pa svaki pozivalac mora
 *     da je proveri i prosledi dalje -- izuzetak to radi sam.
 * U C++ verziji ništa od ovoga ne postoji: destruktori se pozivaju sami, u
 * obrnutom redosledu, na svakom izlazu. Cena u mašinskom kodu je ista --
 * kompajler na svakom izlazu ubaci isti fclose() koji ovde pišeš ručno.
 */
#include <stdio.h>

static int open_count = 0;

static FILE* open_file(const char* path, const char* mode) {
    FILE* f = fopen(path, mode);
    if (f != NULL) {
        ++open_count;
        printf("  [open  %s]\n", path);
    }
    return f;
}

static void close_file(FILE* f, const char* path) {
    if (f == NULL) return;
    fclose(f);
    --open_count;
    printf("  [close %s]\n", path);
}

/* 0 = uspeh, -1 = greška. */
static int copy_and_fail(const char* from, const char* to) {
    int rc = -1;
    FILE* in = NULL;   /* MORA NULL pre prvog goto-a */
    FILE* out = NULL;
    char buf[64];

    in = open_file(from, "r");
    if (in == NULL) goto cleanup;

    out = open_file(to, "w");
    if (out == NULL) goto cleanup;

    while (fgets(buf, sizeof buf, in) != NULL) {
        if (fputs(buf, out) == EOF) goto cleanup;
    }
    printf("  copied, now failing\n");
    goto cleanup;      /* "validation failed" -- u C++ je ovo bio throw */

    /* rc = 0;  -- uspešan put bi stigao ovde */

cleanup:
    close_file(out, to);   /* ručno: obrnuto od otvaranja */
    close_file(in, from);
    return rc;
}

int main(void) {
    const char* a = "step1c_a.txt";
    const char* b = "step1c_b.txt";

    FILE* f = open_file(a, "w");
    if (f == NULL) return 1;
    fputs("first\nsecond\n", f);
    close_file(f, a);

    if (copy_and_fail(a, b) != 0) {
        printf("  copy_and_fail returned error (open files: %d)\n", open_count);
    }
    if (copy_and_fail("no_such_dir/x.txt", b) != 0) {
        printf("  missing file: error (open files: %d)\n", open_count);
    }

    remove(a);
    remove(b);
    printf("== done, open files: %d\n", open_count);
    return 0;
}

/* EXPECTED OUTPUT
  [open  step1c_a.txt]
  [close step1c_a.txt]
  [open  step1c_a.txt]
  [open  step1c_b.txt]
  copied, now failing
  [close step1c_b.txt]
  [close step1c_a.txt]
  copy_and_fail returned error (open files: 0)
  missing file: error (open files: 0)
== done, open files: 0
*/
