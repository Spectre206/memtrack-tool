Here is the fully updated `README.md` reflecting your recent architectural shifts, the new `logs/` directory structure, and the upgraded automated Valgrind testing pipeline. 

```markdown
# Memory Leak Detection Tool

**System Programming Project | UET Peshawar**
**Team: Adeel · Faizullah · Qasim**

A lightweight, runtime memory leak detector for C programs on Linux. 
This tool intercepts `malloc()` and `free()` calls to track dynamic memory usage at runtime, maintaining an internal data structure of active allocations to generate detailed, color-coded reports at program termination.

---

## 🚀 Overview

Unlike basic preload-only tools, this system is designed with a **Linked Mode** architecture as its primary tracking mechanism, combining:

* **Compile-time macro wrapping:** Injects precise `__FILE__`, `__LINE__`, and `__func__` metadata directly into the tracking nodes.
* **Runtime function interposition:** Uses `dlsym(RTLD_NEXT)` for transparent interception of heap allocations.

---

## ⚙️ Modes of Operation

| Mode | Description | Use Case |
| :--- | :--- | :--- |
| **Linked Mode (Primary)** | Compile with `-lmemtrack` and include `memtrack.h` | Full file, line, and function tracking for internal development. |
| **LD_PRELOAD Mode (Secondary)** | Inject library at runtime (`LD_PRELOAD=./libmemtrack.so`) | External binaries where source code cannot be modified (loses precise line numbers). |

---

## ⚡ Quick Start

### 1. Build the tracking library and directories
```bash
make
```
*Note: This generates `build/libmemtrack.so` and prepares the `logs/` directory.*

### 2. Run your program (Linked Mode)
Add `#include "include/memtrack.h"` to your source files, compile with `-lmemtrack`, and execute:
```bash
LD_LIBRARY_PATH=./build ./your_program
```

### 3. Output Modes & Logging
```bash
# Export report to a plain text log file
MEMTRACK_LOG=logs/leaks.txt LD_LIBRARY_PATH=./build ./your_program

# Export report as a data-rich CSV
MEMTRACK_CSV=logs/leaks.csv LD_LIBRARY_PATH=./build ./your_program
```

---

## 🔍 Internal Architecture

```text
Application Code
      │
      ▼
#define malloc(size) → tracked_malloc(size, __FILE__, __LINE__, __func__)
      │
      ▼
tracked_malloc()
      │
      ├── real_malloc() via dlsym(RTLD_NEXT)
      ├── create AllocationRecord
      ├── insert into linked list (mutex protected)
      ▼
Program Execution
      ▼
tracked_free()
      │
      ├── O(n) scan to remove record from list
      ├── update global freed bytes counter
      ▼
Program Exit (__attribute__((destructor)))
      ▼
generate_report()
```

---

## ✨ Key Features

* 🔁 **Runtime Interception:** Utilizes `dlsym(RTLD_NEXT)` to route allocations.
* 📍 **Precision Tracking:** Macro wrapping captures precise file, line, and function origins.
* 🧵 **Thread-Safety:** Fully protected singly-linked list using `pthread_mutex`.
* 🧠 **Re-entrancy Guard:** Custom static fallback allocator handles recursive internal `dlsym` setup calls gracefully.
* 📊 **Severity Classification:** Automatically categorizes leaks (SMALL < 1KB, MEDIUM < 1MB, LARGE ≥ 1MB).
* ⚠️ **Anomaly Detection:** Flags invalid or double `free()` calls without crashing the target program.

---

## 🧪 Test Suite & Validation

The project includes a robust, automated test suite that directly pits our tracking logic against **Valgrind Memcheck** to ensure 100% mathematical accuracy. 

To run the full suite (samples and automated tests):
```bash
make run-samples
make run-tests
```

### Automated Validation Pipeline
The `run_tests.sh` script automatically compiles two versions of every test: a *Linked* binary for our tool, and a *Pure* binary (`-DNO_MEMTRACK`) for an unadulterated Valgrind baseline. 

**Sample Output:**
```text
── test3 (Multiple Leaks) ─────────────────────────
  Our tool  : 10 block(s) leaked (640 bytes)
  Valgrind  : 10 block(s) leaked (640 bytes)

── test4 (Large Leak) ─────────────────────────────
  Our tool  : 1 block(s) leaked (2.00 MB)
  Valgrind  : 1 block(s) leaked (2,097,152 bytes)
```
*All detailed execution logs for both Valgrind and Memtrack are permanently saved in the `logs/` directory after testing.*

---

## 📁 Project Structure

```text
memtrack-tool/
├── include/
│   └── memtrack.h          # Shared team contract & macros
├── src/
│   ├── memtrack.c          # Core interception & list logic
│   └── reporter.c          # Severity classification & UI output
├── samples/                # Visual formatting demonstrations
├── tests/                  # Valgrind validation suite & bash runner
├── build/                  # Compiled artifacts (.so and executables)
├── logs/                   # Generated reports and Valgrind comparisons
└── Makefile                # Build system automation
```

---

## ⚠️ Limitations

* Tracks `malloc()` and `free()` exclusively (`calloc` and `realloc` are currently out of scope).
* Invalid pointer detection relies on heuristic untracked-pointer warnings.
* Minor execution overhead introduced by `pthread_mutex` locking during concurrent allocation heavy workloads.
* Does not track stack or static segment memory.

---

## 👥 Team Responsibilities

| Member | Core Module | Focus Areas |
| :--- | :--- | :--- |
| **Adeel** | Memory Tracking | `tracked_malloc`, linked list management, mutex safety, global state |
| **Faizullah** | Reporting System | `generate_report`, severity logic, ANSI colors, CSV/File logging |
| **Qasim** | Testing & Analysis | Valgrind baseline comparison, bash scripting, overhead benchmarking |

---

## 🛠 Requirements

* **OS:** Linux (Ubuntu recommended)
* **Compiler:** GCC
* **Libraries:** `libdl`, `libpthread`
* **Validation:** Valgrind (required to run the automated test script)

---

## 📜 License

Academic project — free to use for learning, experimentation, and system programming education.
```