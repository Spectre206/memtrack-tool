<h1 align="center">MemTrack Tool</h1>
<p align="center"><i>A lightweight, runtime memory leak detector for C programs on Linux</i></p>

<p align="center">
  <img src="https://img.shields.io/badge/-C-A8B9CC?style=flat-square&logo=c&logoColor=white" />
  <img src="https://img.shields.io/badge/-Linux-FCC624?style=flat-square&logo=linux&logoColor=black" />
  <img src="https://img.shields.io/badge/-Make-A42E2B?style=flat-square&logo=gnu&logoColor=white" />
  <img src="https://img.shields.io/badge/-Systems%20Programming-2E86AB?style=flat-square" />
</p>

<p align="center">
  <a href="#overview">Overview</a> ·
  <a href="#how-it-works">How It Works</a> ·
  <a href="#modes-of-operation">Modes of Operation</a> ·
  <a href="#quick-start">Quick Start</a> ·
  <a href="#requirements">Requirements</a>
</p>

---

## Overview

This tool intercepts `malloc()` and `free()` calls to track dynamic memory usage at runtime, maintaining an internal data structure of active allocations to generate detailed, color-coded reports at program termination.

Unlike basic preload-only tools, MemTrack is built around a **Linked Mode** architecture as its primary tracking mechanism — combining compile-time instrumentation with runtime interception for more precise diagnostics than either technique offers alone.

---

## How It Works

MemTrack combines two complementary tracking techniques:

- **Compile-time macro wrapping** — injects precise `__FILE__`, `__LINE__`, and `__func__` metadata directly into each tracking node, so every allocation can be traced back to its exact source location.
- **Runtime function interposition** — uses `dlsym(RTLD_NEXT)` to transparently intercept heap allocation calls without modifying the underlying allocator.

Together, these let MemTrack trace each leak back to its exact allocation site — file, line, and function.

---

## Modes of Operation

| Mode | Description | Use Case |
| :--- | :--- | :--- |
| **Linked Mode** (Primary) | Compile with `-lmemtrack` and include `memtrack.h` | Full file, line, and function tracking for internal development |
| **LD_PRELOAD Mode** (Secondary) | Inject the library at runtime — `LD_PRELOAD=./libmemtrack.so` | External binaries where source code can't be modified (loses precise line numbers) |

---

## Quick Start

**1. Build the tracking library**
```bash
make
```

**2. Track your own source — Linked Mode**

Compile with `-lmemtrack` and `#include "memtrack.h"` to get full file, line, and function-level tracking.

**3. Or track an external binary — LD_PRELOAD Mode**
```bash
LD_PRELOAD=./libmemtrack.so ./your_program
```

**4. Read the report**

When the program exits, MemTrack prints a color-coded leak report to the terminal automatically.

---

## Requirements

- Linux (relies on `dlsym(RTLD_NEXT)` for interposition)
- GCC/Clang + Make

---

<p align="center"><i><a href="https://github.com/Spectre206">@Spectre206</a></i></p>
