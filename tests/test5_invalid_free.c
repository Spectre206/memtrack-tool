/* ============================================================
 * tests/test5_invalid_free.c  —  Test 5: Double-Free Warning
 * Owner    : Qasim
 * Expected : WARNING printed for second free() — no crash
 *            1 SMALL leak (the first malloc that is never freed)
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include "../include/memtrack.h"

int main(void) {
    char *p = malloc(32);
    free(p);          /* correct first free */
    free(p);          /* double-free — should print WARNING, not crash */

    void *q = malloc(64);
    (void)q;          /* genuine leak */

    return 0;
    /*
     * Expected:
     *   WARNING: free() called on untracked pointer ...
     *   LEAK #2 — 64 bytes — SMALL
     */
}
