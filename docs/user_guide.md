# User Guide — Memory Leak Detection Tool

**Owner: Qasim** — *Finalized Documentation*

---

## 1. Prerequisites

* Linux (Ubuntu 20.04+ x86-64)
* GCC installed: `sudo apt install gcc`
* Make installed: `sudo apt install make`
* Valgrind (for validation): `sudo apt install valgrind`

---

## 2. Build the Tool

```bash
git clone https://github.com/Spectre206/memtrack-tool.git
cd memtrack-tool
make
```

---

## 3. System-Wide Installation (Recommended)

[cite_start]To use the tool seamlessly across any project without managing complex `-I` and `-L` compilation flags, install it natively into the system library paths[cite: 622].

**Step 1: Copy the files to system directories**
```bash
sudo cp ./include/memtrack.h /usr/local/include/
sudo cp ./build/libmemtrack.so /usr/local/lib/
```

**Step 2: Update the dynamic linker cache**
```bash
sudo ldconfig
```

**Step 3: Usage in external projects**
[cite_start]Include the header using standard angle brackets and compile with the `-lmemtrack` flag[cite: 630, 631, 632].
```c
#include <memtrack.h>
```
```bash
gcc -Wall -g your_program.c -o your_program -lmemtrack
./your_program
```

---

## 4. Local Directory Usage (Without System Install)

[cite_start]If running locally within the project structure, use the **Linked Mode** for precise file and line information[cite: 671, 672].

[cite_start]Add `#include "memtrack.h"` to the top of your `.c` file, compile with the library, and run using `LD_LIBRARY_PATH`[cite: 673, 674]:
```bash
LD_LIBRARY_PATH=./build ./build/your_program
```

---

## 5. Output Modes & Logging

[cite_start]You can export the results to a plain text log file and a data-rich CSV by setting environment variables before execution[cite: 686].

```bash
# Export reports to the logs directory
MEMTRACK_LOG=logs/report.txt MEMTRACK_CSV=logs/leaks.csv LD_LIBRARY_PATH=./build ./build/your_program
```

---

## 6. Reading the Report

[cite_start]The tool automatically classifies leaked blocks into three severity levels based on size[cite: 568]:
* [cite_start]**SMALL (Green):** `< 1,024 bytes` [cite: 570]
* [cite_start]**MEDIUM (Yellow):** `1,024 ≤ size < 1,048,576 bytes` [cite: 570]
* [cite_start]**LARGE (Red):** `≥ 1,048,576 bytes (1MB+)` [cite: 570]

**Sample Output:**
```text
════════════════════════════════════════════════════════════════
   MEMORY LEAK DETECTION TOOL — REPORT
════════════════════════════════════════════════════════════════

  SUMMARY
  ────────────────────────────────────────────────────────────────
  Total malloc() calls   : 2
  Total bytes allocated  : 5.00 MB
  Total bytes freed      : 64 bytes
  Leaked blocks          : 1
  Leaked bytes           : 5.00 MB

  LEAKED BLOCKS
  ────────────────────────────────────────────────────────────────
  LEAK #1 — 5.00 MB  [LARGE]
    Address : 0x7eacb4aff010
    File    : samples/sample_c.c
    Line    : 15
    Func    : main()
    Time    : 2026-04-18 15:49:46
```

---

## 7. Run the Test Suite

```bash
# To run the visual sample programs
make samples
make run-samples

# To run the automated Valgrind validation suite
make tests
make run-tests
```

---

## 8. Valgrind Comparison Results

[cite_start]The automated `run_tests.sh` suite mathematically proved the tool's accuracy against Valgrind Memcheck[cite: 597].

| Test Program | Expected Leaks | Our Tool | Valgrind Baseline | Result |
| :--- | :--- | :--- | :--- | :--- |
| **test1** — No Leak | 0 | [cite_start]0 blocks (0 bytes) [cite: 600] | [cite_start]0 blocks (0 bytes) [cite: 600] | [cite_start]**PASS** [cite: 600] |
| **test2** — Single Leak | 1 | [cite_start]1 block (50 bytes) [cite: 600] | [cite_start]1 block (50 bytes) [cite: 600] | [cite_start]**PASS** [cite: 600] |
| **test3** — Multi Leak | 10 | [cite_start]10 blocks (640 bytes) [cite: 600] | [cite_start]10 blocks (640 bytes) [cite: 600] | [cite_start]**PASS** [cite: 600] |
| **test4** — Large Leak | 1 | [cite_start]1 block (2.00 MB) [cite: 600] | [cite_start]1 block (2,097,152 bytes) [cite: 600] | [cite_start]**PASS** [cite: 600] |
| **test5** — Double Free | 1 + WARNING | [cite_start]1 block (64 bytes) + WARN [cite: 600] | [cite_start]1 block (64 bytes) [cite: 600] | [cite_start]**PASS** [cite: 600] |
| **test6** — Multi-thread | 4 | [cite_start]4 blocks (1.00 KB) [cite: 600] | [cite_start]4 blocks (1,024 bytes) [cite: 600] | [cite_start]**PASS** [cite: 600] |

---

## 9. Known Limitations

* Intercepts `malloc()` and `free()` exclusively — `calloc()` and `realloc()` are not tracked in this version.
* Linux x86-64 only (relies on Linux-specific dynamic linker hooking).
* Does not track stack allocations (only heap memory).
* Precise file and line information requires modifying target source code to `#include "memtrack.h"` and compiling with `-lmemtrack`.