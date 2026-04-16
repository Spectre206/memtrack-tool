/* ============================================================
 * samples/sample_b.c  —  Sample B: Multiple Small Leaks
 * Owner    : Faizullah
 * Expected : 5 SMALL leaks reported
 * ============================================================ */

#include <stdio.h>
#include <string.h>
#include "../include/memtrack.h"

/* Simulates a function that allocates but never frees */
static char *load_config(const char *key) {
    char *buf = malloc(128);     /* intentional leak */
    if (!buf) return NULL;
    snprintf(buf, 128, "value_for_%s", key);
    return buf;
    /* caller never frees — classic pattern */
}

int main(void) {
    printf("Sample B: Allocating without freeing (5 small leaks)...\n");

    const char *keys[] = {"host", "port", "user", "pass", "db"};
    for (int i = 0; i < 5; i++) {
        char *val = load_config(keys[i]);
        printf("  Config %s = %s  (not freed)\n", keys[i], val);
        /* deliberately NOT calling free(val) */
    }

    printf("Sample B complete. Expect: 5 SMALL leaks (128 bytes each).\n");
    return 0;
}
