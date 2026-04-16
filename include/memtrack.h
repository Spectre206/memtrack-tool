/* ============================================================
 * memtrack.h  —  Shared Team Contract
 * Memory Leak Detection Tool
 * Team: Adeel · Faizullah · Qasim
 * Version: 1.0 | April 2026
 * DO NOT MODIFY without team consensus
 * ============================================================ */

#ifndef MEMTRACK_H
#define MEMTRACK_H

#include <stddef.h>      /* size_t   */
#include <time.h>        /* time_t   */
#include <pthread.h>     /* pthread_mutex_t */

/* ── Constants ─────────────────────────────────────────────── */
#define MAX_FILE_LEN  256
#define MAX_FUNC_LEN   64

/* ── Severity Levels — used by Faizullah's reporter ───────── */
typedef enum {
    SEVERITY_SMALL  = 0,   /* size < 1,024 bytes        (GREEN)  */
    SEVERITY_MEDIUM = 1,   /* 1,024 – 1,048,575 bytes   (YELLOW) */
    SEVERITY_LARGE  = 2,   /* >= 1,048,576 bytes        (RED)    */
} LeakSeverity;

/* ── Core Record — one node in Adeel's linked list ────────── */
typedef struct AllocationRecord {
    unsigned long           id;                  /* sequential ID         */
    void                   *address;             /* pointer from malloc() */
    size_t                  size;                /* bytes requested       */
    char                    file[MAX_FILE_LEN];  /* __FILE__              */
    int                     line;                /* __LINE__              */
    char                    func[MAX_FUNC_LEN];  /* __func__              */
    time_t                  timestamp;           /* time of allocation    */
    struct AllocationRecord *next;               /* linked list pointer   */
} AllocationRecord;

/* ── Global State — Adeel owns, Faizullah reads via report fn  */
extern AllocationRecord  *head;
extern unsigned long      id_counter;
extern size_t             total_bytes_alloc;
extern size_t             total_bytes_freed;
extern pthread_mutex_t    lock;

/* ── Macro Wrappers — intercept calls at compile time ──────── */
/* SCOPE: malloc() and free() ONLY                               */
#define malloc(size)   tracked_malloc((size), __FILE__, __LINE__, __func__)
#define free(ptr)      tracked_free((ptr))

/* ── Function Declarations ─────────────────────────────────── */
/* Adeel implements both of these in src/memtrack.c              */
void *tracked_malloc(size_t size, const char *file, int line, const char *func);
void  tracked_free  (void *ptr);

/* Faizullah implements this in src/reporter.c                   */
void  generate_report(AllocationRecord *head,
                      unsigned long     total_alloc,
                      size_t            total_bytes_alloc,
                      size_t            total_bytes_freed);

#endif /* MEMTRACK_H */
