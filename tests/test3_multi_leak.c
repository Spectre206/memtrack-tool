/* ============================================================
 * tests/test3_multi_leak.c  —  Test 3: Multiple Leaks
 * Owner    : Qasim
 * Expected : 10 SMALL leaks (64 bytes each = 640 bytes total)
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include "../include/memtrack.h"

int main(void) {
    for (int i = 0; i < 10; i++) {
        void *p = malloc(64);
        (void)p;    /* never freed */
    }
    return 0;
    /* Expected: 10 SMALL leaks, each 64 bytes, all pointing to this loop */
}
