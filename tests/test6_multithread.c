/* ============================================================
 * tests/test6_multithread.c  —  Test 6: Multi-threaded Leaks
 * Owner    : Qasim
 * Purpose  : Verifies mutex correctness — 4 threads each
 *            malloc() twice; only first alloc per thread freed.
 * Expected : 4 SMALL leaks (one unfreed block per thread)
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/memtrack.h"

#define NUM_THREADS 4

static void *thread_work(void *arg) {
    int id = *(int *)arg;

    /* First allocation — will be freed */
    void *p1 = malloc(128);
    if (p1) {
        /* do some "work" */
        free(p1);   /* correctly freed */
    }

    /* Second allocation — intentional leak */
    void *p2 = malloc(256);
    (void)p2;       /* never freed */

    printf("  Thread %d: freed p1, leaked p2 (256 bytes)\n", id);
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    printf("Launching %d threads...\n", NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, thread_work, &ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads done.\n");
    return 0;
    /*
     * Expected: 4 SMALL leaks — 256 bytes each — from thread_work()
     * Mutex must prevent any corruption across concurrent inserts/removes.
     */
}
