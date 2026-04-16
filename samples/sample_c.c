/* ============================================================
 * samples/sample_c.c  —  Sample C: One Critical Large Leak
 * Owner    : Faizullah
 * Expected : 1 LARGE leak (5 MB) reported in RED
 * ============================================================ */

#include <stdio.h>
#include <string.h>
#include "../include/memtrack.h"

int main(void) {
    printf("Sample C: Allocating 5 MB without freeing...\n");

    size_t big = 5 * 1024 * 1024;   /* 5 MB */
    char *buffer = malloc(big);
    if (!buffer) {
        printf("malloc failed\n");
        return 1;
    }

    /* Simulate some work with the buffer */
    memset(buffer, 'A', big);
    printf("  Allocated 5 MB at %p — never freed.\n", (void *)buffer);

    /* Also one small correctly-freed block to show contrast */
    char *msg = malloc(64);
    if (msg) {
        snprintf(msg, 64, "This one gets freed.");
        printf("  Also allocated 64 bytes: \"%s\"\n", msg);
        free(msg);
        printf("  Small block freed correctly.\n");
    }

    printf("Sample C complete. Expect: 1 LARGE leak (5 MB).\n");
    return 0;
    /* buffer is never freed */
}
