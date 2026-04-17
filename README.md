Here is the updated `README.md` with the new Git troubleshooting section added at the bottom. I have used generic placeholders (like `<commit-hash>`) so it remains clean and professional for your repository.

```markdown
# Memory Leak Detection Tool

**System Programming Project | [University Name Placeholder]**
**Team: [Team Member 1] · [Team Member 2] · [Team Member 3]**

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
LD_LIBRARY_PATH=./build ./<your_binary_name>
```

### 3. Output Modes & Logging
```bash
# Export report to a plain text log file
MEMTRACK_LOG=logs/leaks.txt LD_LIBRARY_PATH=./build ./<your_binary_name>

# Export report as a data-rich CSV
MEMTRACK_CSV=logs/leaks.csv LD_LIBRARY_PATH=./build ./<your_binary_name>
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
[Project Root]/
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

## 🚑 Git Troubleshooting: Undoing Commits

During collaborative development, mistakes happen. Here is how to safely undo commits depending on the scenario:

### 1. Keep Changes (Soft Reset)
If you committed locally but forgot a file or need to make edits. This moves the branch back one commit but keeps your files exactly as they are in your staging area.
```bash
git reset --soft HEAD~1
```

### 2. Discard Changes (Hard Reset)
**⚠️ Destructive Action:** If you want to permanently delete the last commit and wipe out all code changes inside it.
```bash
git reset --hard HEAD~1
```

### 3. Undo a Pushed Commit (Safe for Teams)
If the commit has already been pushed to the remote repository. This creates a *new* commit that perfectly reverses the bad one, preserving the project's history for the rest of the team.
```bash
git log  # Find the hash of the commit you want to undo
git revert <commit-hash>
```

### 4. Edit the Commit Message
If you simply made a typo in the commit message and haven't pushed it yet.
```bash
git commit --amend -m "<Your corrected commit message>"
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
| **[Team Member 1]** | Memory Tracking | `tracked_malloc`, linked list management, mutex safety, global state |
| **[Team Member 2]** | Reporting System | `generate_report`, severity logic, ANSI colors, CSV/File logging |
| **[Team Member 3]** | Testing & Analysis | Valgrind baseline comparison, bash scripting, overhead benchmarking |

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