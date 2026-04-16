/* ============================================================
 * samples/sample_a.c  —  Sample A: No Leaks
 * Owner    : Faizullah
 * Expected : Clean report — "No memory leaks detected!"
 * ============================================================ */

#include <stdio.h>
#include <string.h>
#include "../include/memtrack.h"

int main(void) {
    printf("Sample A: Allocating and correctly freeing memory...\n");

    char *name = malloc(64);
    if (!name) return 1;
    strcpy(name, "Hello, Memory Tracker!");
    printf("  Allocated 64 bytes at %p: \"%s\"\n", (void *)name, name);
    free(name);
    printf("  Freed.\n");

    int *numbers = malloc(10 * sizeof(int));
    if (!numbers) return 1;
    for (int i = 0; i < 10; i++) numbers[i] = i * i;
    printf("  Allocated int array at %p\n", (void *)numbers);
    free(numbers);
    printf("  Freed.\n");

    printf("Sample A complete. Expect: zero leaks.\n");
    return 0;
}
