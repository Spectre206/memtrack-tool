# Memory Leak Detection Tool

**System Programming Project | UET Peshawar**
**Team: Adeel · Faizullah · Qasim**

A lightweight, runtime memory leak detector for C programs on Linux.
Supports both **linked mode (with file/line tracking)** and **LD_PRELOAD mode (no recompilation required)**.

---

## 🚀 Overview

This tool intercepts `malloc()` and `free()` calls to track dynamic memory usage at runtime.
It maintains an internal data structure of all active allocations and generates a detailed report at program termination.

Unlike basic preload-only tools, this system combines:

* **Compile-time macro wrapping** → precise file/line/function tracking
* **Runtime function interposition (`dlsym`)** → transparent interception

---

## ⚙️ Modes of Operation

| Mode                          | Description                                  | Use Case                              |
| ----------------------------- | -------------------------------------------- | ------------------------------------- |
| **Linked Mode (Recommended)** | Compile with `-lmemtrack` and include header | Full file/line/function tracking      |
| **LD_PRELOAD Mode**           | Inject library at runtime                    | External binaries (no source changes) |

---

## ⚡ Quick Start

### 1. Build the library

```bash
make
```

### 2. Run (Linked Mode — recommended)

```bash
LD_LIBRARY_PATH=./build ./your_program
```

### 3. Optional features

```bash
# Save report to file
MEMTRACK_LOG=leaks.txt LD_LIBRARY_PATH=./build ./your_program

# Export as CSV
MEMTRACK_CSV=leaks.csv LD_LIBRARY_PATH=./build ./your_program
```

---

## 📌 Enabling File/Line Tracking

Add this at the top of your source file:

```c
#include "include/memtrack.h"
```

This enables macro-based interception for precise debugging information.

---

## 🔍 Internal Architecture

```text
Application Code
      │
      ▼
#define malloc → tracked_malloc()
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
      ├── remove record from list
      ├── update freed bytes
      ▼
Program Exit (Destructor)
      ▼
generate_report()
```

---

## ✨ Key Features

* 🔁 Runtime interception using `dlsym(RTLD_NEXT)`
* 📍 File/line/function tracking via macro wrapping
* 🧵 Thread-safe tracking using `pthread_mutex`
* 🧠 Re-entrancy-safe initialization (handles `dlsym` recursion)
* 🧱 Custom fallback allocator for early-stage allocations
* 📊 Leak severity classification (SMALL / MEDIUM / LARGE)
* 📁 Optional log file and CSV export
* ⚠️ Detection of invalid or double `free()` calls

---

## 📊 Sample Output

```
SUMMARY
Total malloc() calls   : 2
Total bytes allocated  : 5.00 MB
Total bytes freed      : 64 bytes
Leaked blocks          : 1
Leaked bytes           : 5.00 MB

LEAK #1 — 5.00 MB  [LARGE]
  File    : samples/sample_c.c
  Line    : 15
  Func    : main()
```

---

## 🧪 Test Suite & Validation

The project includes an automated test suite covering:

* No leaks
* Single leak
* Multiple leaks
* Large allocations
* Invalid free detection
* Multi-threaded scenarios

### Validation

The tool is validated against **Valgrind**:

| Test Case      | Expected       | Our Tool | Valgrind |
| -------------- | -------------- | -------- | -------- |
| No Leak        | 0              | ✅        | ✅        |
| Single Leak    | 1              | ✅        | ✅        |
| Multiple Leaks | 10             | ✅        | ✅        |
| Large Leak     | 1              | ✅        | ✅        |
| Invalid Free   | Warning + Leak | ✅        | ✅        |
| Multithreaded  | 4              | ✅        | ✅        |

---

## 📁 Project Structure

```
memtrack-tool/
├── include/
│   └── memtrack.h
├── src/
│   ├── memtrack.c
│   └── reporter.c
├── samples/
├── tests/
├── docs/
├── build/
└── Makefile
```

---

## ⚠️ Limitations

* Only tracks `malloc()` and `free()` (not `calloc`, `realloc`)
* Requires recompilation for full file/line tracking
* Invalid pointer detection is heuristic
* Does not track stack or static allocations
* Adds minor runtime overhead due to mutex locking

---

## 🧠 Technical Highlights

* Function interposition using `dlsym(RTLD_NEXT)`
* Constructor/destructor-based lifecycle management
* Lock-protected linked list for allocation tracking
* Safe handling of early allocations via fallback buffer
* Designed to avoid recursion during initialization

---

## 👥 Team Responsibilities

| Member    | Module               |
| --------- | -------------------- |
| Adeel     | Memory Tracking Core |
| Faizullah | Reporting System     |
| Qasim     | Testing & Validation |

---

## 🛠 Requirements

* Linux (Ubuntu recommended)
* GCC
* `libdl`
* `libpthread`
* Valgrind (optional, for validation)

---

## 📌 Future Work

* Support for `calloc`, `realloc`, `strdup`
* Leak grouping and aggregation
* Performance optimization (lock-free structures)
* Integration with visualization tools

---

## 📜 License

Academic project — free to use for learning and experimentation.
