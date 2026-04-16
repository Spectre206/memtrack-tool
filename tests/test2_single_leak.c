/* ============================================================
 * tests/test2_single_leak.c  —  Test 2: Single Small Leak
 * Owner    : Qasim
 * Expected : 1 SMALL leak (50 bytes) — from main.c line ~14
 * ============================================================ */

#include <stdio.h>
#include "../include/memtrack.h"

int main(void) {
    char *p = malloc(50);   /* intentional leak */
    (void)p;                /* suppress unused warning */
    return 0;
    /* Expected: LEAK #1 — 50 bytes — SMALL — test2_single_leak.c:14 */
}
