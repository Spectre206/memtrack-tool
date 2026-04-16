# Memory Leak Detection Tool

**System Programming Project | UET Peshawar**
**Team: Adeel · Faizullah · Qasim**

A lightweight runtime memory leak detector for C programs on Linux.
Intercepts `malloc()` and `free()` via `LD_PRELOAD` — no source modification required except one `#include`.

---

## How It Works

1. Our shared library (`libmemtrack.so`) is injected before libc using `LD_PRELOAD`
2. Every `malloc()` call is intercepted, recorded in a linked list with file/line/func info
3. Every `free()` removes the matching record
4. On program exit, remaining records = leaked allocations → full report printed

---

## Quick Start

```bash
# 1. Build the library
make

# 2. Run ANY program with the tracker
LD_PRELOAD=./build/libmemtrack.so ./your_program

# 3. Save report to a log file
MEMTRACK_LOG=leaks.txt LD_PRELOAD=./build/libmemtrack.so ./your_program

# 4. Export leaks as CSV
MEMTRACK_CSV=leaks.csv LD_PRELOAD=./build/libmemtrack.so ./your_program
```

For programs where you want file/line info, add one line at the top:
```c
#include "include/memtrack.h"
```

---

## Build Targets

| Command | Description |
|---|---|
| `make` | Build `libmemtrack.so` |
| `make samples` | Build sample programs A, B, C |
| `make tests` | Build all 6 test programs |
| `make run-samples` | Build + run all samples |
| `make run-tests` | Build + run full test suite |
| `make clean` | Remove build directory |

---

## Project Structure

```
memtrack-tool/
├── include/
│   └── memtrack.h          ← Shared contract (all three include this)
├── src/
│   ├── memtrack.c          ← Adeel: tracked_malloc, tracked_free, linked list, mutex
│   └── reporter.c          ← Faizullah: generate_report, severity, ANSI colors
├── samples/
│   ├── sample_a.c          ← Faizullah: No leaks (clean report)
│   ├── sample_b.c          ← Faizullah: 5 small leaks
│   └── sample_c.c          ← Faizullah: 1 large leak (5 MB)
├── tests/
│   ├── test1_no_leak.c     ← Qasim
│   ├── test2_single_leak.c ← Qasim
│   ├── test3_multi_leak.c  ← Qasim
│   ├── test4_large_leak.c  ← Qasim
│   ├── test5_invalid_free.c← Qasim
│   ├── test6_multithread.c ← Qasim
│   └── run_tests.sh        ← Qasim: automated test runner
├── docs/
│   ├── user_guide.md       ← Qasim: usage documentation
│   └── presentation/       ← Qasim: slides
├── build/                  ← Generated (gitignored)
└── Makefile
```

---

## Severity Levels

| Level | Size | Terminal Color |
|---|---|---|
| SMALL | < 1 KB | 🟢 GREEN |
| MEDIUM | 1 KB – 1 MB | 🟡 YELLOW |
| LARGE | > 1 MB | 🔴 RED |

---

## Team Responsibilities

| Member | Module | Files |
|---|---|---|
| Adeel | Memory Tracking | `src/memtrack.c`, `include/memtrack.h` |
| Faizullah | Reporting System | `src/reporter.c`, `samples/` |
| Qasim | Testing & Analysis | `tests/`, `docs/` |

---

## Git Workflow

```bash
# Clone
git clone https://github.com/YOUR_USERNAME/memtrack-tool.git

# Each member works on their branch
git checkout -b feature/tracker     # Adeel
git checkout -b feature/reporter    # Faizullah
git checkout -b feature/tests       # Qasim

# Merge to dev when module is working
git checkout dev
git merge feature/tracker

# Merge to main only after full team review
git checkout main
git merge dev
```

---

## Requirements

- Linux (Ubuntu 20.04+ recommended)
- GCC
- `libdl` (usually pre-installed)
- `libpthread` (usually pre-installed)
- Valgrind (optional, for test comparison)
