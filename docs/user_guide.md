# User Guide — Memory Leak Detection Tool

> **Owner: Qasim** — fill in screenshots and Valgrind comparison table after testing

---

## 1. Prerequisites

- Linux (Ubuntu 20.04+ x86-64)
- GCC installed: `sudo apt install gcc`
- Make installed: `sudo apt install make`
- Valgrind (for comparison): `sudo apt install valgrind`

---

## 2. Build the Tool

```bash
git clone https://github.com/YOUR_USERNAME/memtrack-tool.git
cd memtrack-tool
make
```

Expected output:
```
  Built: build/libmemtrack.so
```

---

## 3. Run on Any Program

### Without file/line info (zero source changes)
```bash
LD_PRELOAD=./build/libmemtrack.so ./your_program
```

### With file/line info (add one line to source)
Add at the top of your `.c` file:
```c
#include "path/to/memtrack.h"
```
Then compile and run normally with `LD_PRELOAD`.

---

## 4. Reading the Report

The report prints to `stderr` automatically on program exit.

```
════════════════════════════════════════════
   MEMORY LEAK DETECTION TOOL — REPORT
════════════════════════════════════════════

  SUMMARY
  ────────────────────────────────────────
  Total malloc() calls   : 6
  Total bytes allocated  : 640 bytes
  Total bytes freed      : 0 bytes
  Leaked blocks          : 6
  Leaked bytes           : 640 bytes

  LEAKED BLOCKS
  ────────────────────────────────────────
  LEAK #1 — 128 bytes  [SMALL]
    Address : 0x55f3c2a01260
    File    : samples/sample_b.c
    Line    : 22
    Func    : load_config()
    Time    : 2026-04-14 09:30:00
```

---

## 5. Optional Outputs

```bash
# Save report to file
MEMTRACK_LOG=report.txt LD_PRELOAD=./build/libmemtrack.so ./program

# Export CSV
MEMTRACK_CSV=leaks.csv LD_PRELOAD=./build/libmemtrack.so ./program
```

---

## 6. Run the Test Suite

```bash
make tests
make run-tests
```

---

## 7. Valgrind Comparison Table

> **Qasim: fill this in after running all tests**

| Test | Expected Leaks | Our Tool | Valgrind | Match? |
|------|---------------|----------|----------|--------|
| test1_no_leak | 0 | | | |
| test2_single_leak | 1 | | | |
| test3_multi_leak | 10 | | | |
| test4_large_leak | 1 | | | |
| test5_invalid_free | 1 | | | |
| test6_multithread | 4 | | | |

---

## 8. Known Limitations

- Intercepts `malloc()` and `free()` only — `calloc()` and `realloc()` are not tracked
- Linux x86-64 only (LD_PRELOAD is Linux-specific)
- Does not track stack allocations (only heap)
- File and line info requires `#include "memtrack.h"` in the target source
