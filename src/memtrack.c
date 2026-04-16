/* ============================================================
 * memtrack.c  —  Memory Tracking Module
 * Owner    : Adeel
 * Purpose  : Intercepts malloc() and free() via LD_PRELOAD.
 *            Maintains a mutex-protected singly-linked list of
 *            AllocationRecord nodes. On program exit the
 *            destructor calls generate_report().
 * ============================================================ */

#define _GNU_SOURCE          /* needed for RTLD_NEXT             */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>           /* dlsym()                          */
#include <pthread.h>
#include <time.h>

/* Include AFTER stdlib — our macros must NOT wrap internal calls */
/* We undef malloc/free here so this file uses real libc directly */
#include "../include/memtrack.h"
#undef malloc
#undef free

/* ── Real function pointers (resolved once at init) ────────── */
static void *(*real_malloc)(size_t)       = NULL;
static void  (*real_free)(void *)         = NULL;

/* ── Global state (declared extern in memtrack.h) ──────────── */
AllocationRecord *head             = NULL;
unsigned long     id_counter       = 0;
size_t            total_bytes_alloc = 0;
size_t            total_bytes_freed = 0;
pthread_mutex_t   lock             = PTHREAD_MUTEX_INITIALIZER;

/* ── Re-entrancy guard ──────────────────────────────────────── */
/* Prevents recursive calls during dlsym() setup (dlsym itself  */
/* calls malloc internally before real_malloc is resolved).     */
static int initialized = 0;

/* ── Static fallback buffer ────────────────────────────────── */
/* Used if malloc() is called before real_malloc is resolved.   */
static char  fallback_buf[4096];
static size_t fallback_used = 0;

/* ============================================================
 * CONSTRUCTOR — runs once when libmemtrack.so is loaded
 * ============================================================ */
__attribute__((constructor))
static void memtrack_init(void) {
    /* Resolve the real malloc and free from libc */
    real_malloc = dlsym(RTLD_NEXT, "malloc");
    real_free   = dlsym(RTLD_NEXT, "free");

    if (!real_malloc || !real_free) {
        fprintf(stderr, "[memtrack] FATAL: could not resolve real malloc/free\n");
        /* Don't exit — let the program continue, but tracking won't work */
        return;
    }

    initialized = 1;
    fprintf(stderr, "[memtrack] Tracker initialized. Monitoring malloc() and free().\n");
}

/* ============================================================
 * DESTRUCTOR — runs once when the program exits
 * ============================================================ */
__attribute__((destructor))
static void memtrack_shutdown(void) {
    fprintf(stderr, "[memtrack] Tracker shutting down. Generating report...\n");
    generate_report(head, id_counter, total_bytes_alloc, total_bytes_freed);
}

/* ============================================================
 * INTERNAL HELPER — create a new AllocationRecord
 * Called inside tracked_malloc() while mutex is held.
 * ============================================================ */
static AllocationRecord *create_record(void *address, size_t size,
                                        const char *file, int line,
                                        const char *func) {
    /* Use real_malloc directly — NOT our wrapper */
    AllocationRecord *rec = real_malloc(sizeof(AllocationRecord));
    if (!rec) return NULL;

    rec->id        = ++id_counter;
    rec->address   = address;
    rec->size      = size;
    rec->line      = line;
    rec->timestamp = time(NULL);
    rec->next      = NULL;

    strncpy(rec->file, file ? file : "unknown", MAX_FILE_LEN - 1);
    rec->file[MAX_FILE_LEN - 1] = '\0';

    strncpy(rec->func, func ? func : "unknown", MAX_FUNC_LEN - 1);
    rec->func[MAX_FUNC_LEN - 1] = '\0';

    return rec;
}

/* ============================================================
 * tracked_malloc()
 * Called by every malloc() in the target program via macro.
 * ============================================================ */
void *tracked_malloc(size_t size, const char *file, int line, const char *func) {

    /* ── Re-entrancy guard ── */
    if (!initialized || !real_malloc) {
        /* Called before init (e.g. during dlsym). Use fallback buffer. */
        if (fallback_used + size < sizeof(fallback_buf)) {
            void *ptr = fallback_buf + fallback_used;
            fallback_used += size;
            return ptr;
        }
        return NULL;
    }

    /* ── Allocate via real malloc ── */
    void *ptr = real_malloc(size);
    if (!ptr) return NULL;     /* allocation failed — return NULL as normal */

    /* ── Record the allocation ── */
    pthread_mutex_lock(&lock);

    AllocationRecord *rec = create_record(ptr, size, file, line, func);
    if (rec) {
        /* Insert at head — O(1) */
        rec->next = head;
        head      = rec;
        total_bytes_alloc += size;
    }

    pthread_mutex_unlock(&lock);

    return ptr;
}

/* ============================================================
 * tracked_free()
 * Called by every free() in the target program via macro.
 * ============================================================ */
void tracked_free(void *ptr) {

    if (!ptr) return;          /* free(NULL) is always a no-op  */

    if (!initialized || !real_free) {
        /* Called before init — nothing to untrack, just return */
        return;
    }

    /* ── Find and remove the record ── */
    pthread_mutex_lock(&lock);

    AllocationRecord *curr = head;
    AllocationRecord *prev = NULL;
    int found = 0;

    while (curr) {
        if (curr->address == ptr) {
            /* Unlink the node */
            if (prev) prev->next  = curr->next;
            else      head        = curr->next;

            total_bytes_freed += curr->size;

            /* Free the record itself using real free */
            real_free(curr);
            found = 1;
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    pthread_mutex_unlock(&lock);

    if (!found) {
        /* Pointer was never tracked — possible double-free or invalid ptr */
        fprintf(stderr,
            "[memtrack] WARNING: free() called on untracked pointer %p "
            "— possible double-free or pointer not allocated with malloc()\n",
            ptr);
        return;
    }

    /* ── Always call real free ── */
    real_free(ptr);
}
