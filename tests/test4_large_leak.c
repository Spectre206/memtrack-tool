/* ============================================================
 * tests/test4_large_leak.c  —  Test 4: Large Leak
 * Owner    : Qasim
 * Expected : 1 LARGE leak (2 MB) — shown in RED
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include "../include/memtrack.h"

int main(void) {
    void *big = malloc(2 * 1024 * 1024);   /* 2 MB — never freed */
    (void)big;
    return 0;
    /* Expected: LEAK #1 — 2.00 MB — LARGE (RED) */
}
