/* ============================================================
 * reporter.c  —  Reporting System
 * Owner    : Faizullah
 * Purpose  : Implements generate_report(). Receives the
 *            AllocationRecord linked list from Adeel's module
 *            on program exit. Classifies severity, prints a
 *            color-coded terminal report, and optionally writes
 *            a log file and CSV export.
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Include WITHOUT malloc/free macros — reporter uses real libc  */
#include "../include/memtrack.h"
#undef malloc
#undef free

/* ── ANSI Color Codes ───────────────────────────────────────── */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_WHITE   "\033[97m"

/* ── Separator widths ───────────────────────────────────────── */
#define WIDE_SEP  "════════════════════════════════════════════════════════════════\n"
#define THIN_SEP  "────────────────────────────────────────────────────────────────\n"

/* ============================================================
 * get_severity()
 * Returns severity level based on allocation size.
 * ============================================================ */
LeakSeverity get_severity(size_t size) {
    if (size < 1024)     return SEVERITY_SMALL;
    if (size < 1048576)  return SEVERITY_MEDIUM;
    return SEVERITY_LARGE;
}

/* ============================================================
 * severity_label()
 * Returns the display label string for a severity level.
 * ============================================================ */
static const char *severity_label(LeakSeverity sev) {
    switch (sev) {
        case SEVERITY_SMALL:  return "SMALL";
        case SEVERITY_MEDIUM: return "MEDIUM";
        case SEVERITY_LARGE:  return "LARGE";
        default:              return "UNKNOWN";
    }
}

/* ============================================================
 * severity_color()
 * Returns the ANSI color code for a severity level.
 * ============================================================ */
static const char *severity_color(LeakSeverity sev) {
    switch (sev) {
        case SEVERITY_SMALL:  return COLOR_GREEN;
        case SEVERITY_MEDIUM: return COLOR_YELLOW;
        case SEVERITY_LARGE:  return COLOR_RED;
        default:              return COLOR_WHITE;
    }
}

/* ============================================================
 * format_bytes()
 * Converts a byte count to a human-readable string.
 * e.g. 1536 → "1.50 KB"
 * ============================================================ */
static void format_bytes(size_t bytes, char *buf, size_t buflen) {
    if      (bytes >= 1048576) snprintf(buf, buflen, "%.2f MB", bytes / 1048576.0);
    else if (bytes >= 1024)    snprintf(buf, buflen, "%.2f KB", bytes / 1024.0);
    else                       snprintf(buf, buflen, "%zu bytes", bytes);
}

/* ============================================================
 * print_record()
 * Prints one leaked AllocationRecord to the given FILE stream.
 * use_color controls ANSI codes (off for log files).
 * ============================================================ */
static void print_record(FILE *out, const AllocationRecord *rec, int use_color) {
    LeakSeverity sev   = get_severity(rec->size);
    const char *color  = use_color ? severity_color(sev) : "";
    const char *reset  = use_color ? COLOR_RESET : "";
    const char *bold   = use_color ? COLOR_BOLD  : "";
    const char *mag    = use_color ? COLOR_MAGENTA : "";

    char size_str[32];
    format_bytes(rec->size, size_str, sizeof(size_str));

    /* Format timestamp */
    char time_str[32];
    struct tm *tm_info = localtime(&rec->timestamp);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(out, "%s  LEAK #%lu%s — %s%s  [%s%s%s]\n",
            bold, rec->id, reset,
            mag, size_str,
            color, severity_label(sev), reset);
    fprintf(out, "    Address : %p\n",   rec->address);
    fprintf(out, "    File    : %s\n",   rec->file);
    fprintf(out, "    Line    : %d\n",   rec->line);
    fprintf(out, "    Func    : %s()\n", rec->func);
    fprintf(out, "    Time    : %s\n",   time_str);
    fprintf(out, use_color ? THIN_SEP : "---\n");
}

/* ============================================================
 * write_csv()
 * Writes one CSV row per leaked block to the given file path.
 * Columns: id, address, size_bytes, severity, file, line, func, timestamp
 * ============================================================ */
static void write_csv(const char *path, const AllocationRecord *list) {
    FILE *f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "[memtrack] WARNING: could not open CSV file: %s\n", path);
        return;
    }

    fprintf(f, "id,address,size_bytes,severity,file,line,func,timestamp\n");

    const AllocationRecord *rec = list;
    while (rec) {
        char time_str[32];
        struct tm *tm_info = localtime(&rec->timestamp);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

        fprintf(f, "%lu,%p,%zu,%s,%s,%d,%s,%s\n",
                rec->id,
                rec->address,
                rec->size,
                severity_label(get_severity(rec->size)),
                rec->file,
                rec->line,
                rec->func,
                time_str);
        rec = rec->next;
    }

    fclose(f);
    fprintf(stderr, "[memtrack] CSV report written to: %s\n", path);
}

/* ============================================================
 * generate_report()
 * Main entry point. Called by Adeel's destructor on exit.
 * ============================================================ */
void generate_report(AllocationRecord *head,
                     unsigned long     total_alloc,
                     size_t            total_bytes_alloc,
                     size_t            total_bytes_freed) {

    /* ── Count leaked blocks ── */
    unsigned long  leak_count  = 0;
    size_t         leak_bytes  = 0;
    AllocationRecord *rec = head;
    while (rec) {
        leak_count++;
        leak_bytes += rec->size;
        rec = rec->next;
    }

    /* ── Prepare human-readable sizes ── */
    char alloc_str[32], freed_str[32], leaked_str[32];
    format_bytes(total_bytes_alloc, alloc_str, sizeof(alloc_str));
    format_bytes(total_bytes_freed, freed_str, sizeof(freed_str));
    format_bytes(leak_bytes,        leaked_str, sizeof(leaked_str));

    /* ────────────────────────────────────────────────────────
     *  TERMINAL REPORT
     * ──────────────────────────────────────────────────────── */
    fprintf(stderr, "\n");
    fprintf(stderr, COLOR_CYAN COLOR_BOLD WIDE_SEP COLOR_RESET);
    fprintf(stderr, COLOR_CYAN COLOR_BOLD
            "   MEMORY LEAK DETECTION TOOL — REPORT\n" COLOR_RESET);
    fprintf(stderr, COLOR_CYAN COLOR_BOLD WIDE_SEP COLOR_RESET);

    /* Summary */
    fprintf(stderr, COLOR_BOLD "\n  SUMMARY\n" COLOR_RESET);
    fprintf(stderr, THIN_SEP);
    fprintf(stderr, "  Total malloc() calls   : %lu\n",   total_alloc);
    fprintf(stderr, "  Total bytes allocated  : %s\n",    alloc_str);
    fprintf(stderr, "  Total bytes freed      : %s\n",    freed_str);
    fprintf(stderr, "  Leaked blocks          : %lu\n",   leak_count);
    fprintf(stderr, "  Leaked bytes           : %s\n\n",  leaked_str);

    /* ── Clean exit ── */
    if (leak_count == 0) {
        fprintf(stderr,
            COLOR_GREEN COLOR_BOLD
            "  ✔  No memory leaks detected!"
            "  All allocations were freed correctly.\n"
            COLOR_RESET);
        fprintf(stderr, COLOR_CYAN WIDE_SEP COLOR_RESET);
        fprintf(stderr, "\n");
        return;
    }

    /* ── Print each leaked block ── */
    fprintf(stderr, COLOR_BOLD "  LEAKED BLOCKS\n" COLOR_RESET);
    fprintf(stderr, THIN_SEP);

    rec = head;
    while (rec) {
        print_record(stderr, rec, 1 /* use_color = yes */);
        rec = rec->next;
    }

    fprintf(stderr, COLOR_CYAN WIDE_SEP COLOR_RESET);
    fprintf(stderr, "\n");

    /* ────────────────────────────────────────────────────────
     *  OPTIONAL LOG FILE  (set MEMTRACK_LOG=/path/to/file)
     * ──────────────────────────────────────────────────────── */
    const char *log_path = getenv("MEMTRACK_LOG");
    if (log_path) {
        FILE *logf = fopen(log_path, "w");
        if (logf) {
            fprintf(logf, "MEMORY LEAK DETECTION TOOL — REPORT\n");
            fprintf(logf, WIDE_SEP);
            fprintf(logf, "Total malloc() calls  : %lu\n", total_alloc);
            fprintf(logf, "Total bytes allocated : %s\n",  alloc_str);
            fprintf(logf, "Total bytes freed     : %s\n",  freed_str);
            fprintf(logf, "Leaked blocks         : %lu\n", leak_count);
            fprintf(logf, "Leaked bytes          : %s\n",  leaked_str);
            fprintf(logf, THIN_SEP);

            rec = head;
            while (rec) {
                print_record(logf, rec, 0 /* use_color = no */);
                rec = rec->next;
            }
            fclose(logf);
            fprintf(stderr, "[memtrack] Log file written to: %s\n", log_path);
        } else {
            fprintf(stderr, "[memtrack] WARNING: could not open log file: %s\n", log_path);
        }
    }

    /* ────────────────────────────────────────────────────────
     *  OPTIONAL CSV EXPORT  (set MEMTRACK_CSV=/path/to/file)
     * ──────────────────────────────────────────────────────── */
    const char *csv_path = getenv("MEMTRACK_CSV");
    if (csv_path) {
        write_csv(csv_path, head);
    }
}
