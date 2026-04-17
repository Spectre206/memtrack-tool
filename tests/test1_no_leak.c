/* ============================================================
 * tests/test1_no_leak.c  —  Test 1: No Leaks
 * Owner    : Qasim
 * Expected : Clean report — zero leaks, zero false positives
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include "../include/memtrack.h"

int main(void) {
    void *p1 = malloc(100);
    void *p2 = malloc(200);
    void *p3 = malloc(300);

    free(p1);
    free(p2);
    free(p3);

    return 0;
    /* Expected: No memory leaks detected! */
}
