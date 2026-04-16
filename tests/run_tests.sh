#!/bin/bash
# ============================================================
# tests/run_tests.sh  —  Test Runner
# Owner    : Qasim
# Purpose  : Runs all 6 tests under our tool and under Valgrind.
#            Measures overhead. Prints a comparison table.
# Usage    : bash tests/run_tests.sh   OR   make run-tests
# ============================================================

LIB="./build/libmemtrack.so"
VALGRIND_CMD="valgrind --tool=memcheck --leak-check=full --error-exitcode=1"

# Colors
RED='\033[31m'
GREEN='\033[32m'
YELLOW='\033[33m'
CYAN='\033[36m'
BOLD='\033[1m'
RESET='\033[0m'

# Check lib exists
if [ ! -f "$LIB" ]; then
    echo -e "${RED}ERROR: $LIB not found. Run 'make' first.${RESET}"
    exit 1
fi

# Check Valgrind available
HAVE_VALGRIND=0
if command -v valgrind &> /dev/null; then
    HAVE_VALGRIND=1
else
    echo -e "${YELLOW}WARNING: valgrind not found. Skipping Valgrind comparison.${RESET}"
fi

echo ""
echo -e "${CYAN}${BOLD}════════════════════════════════════════════════════════════════${RESET}"
echo -e "${CYAN}${BOLD}   MEMORY LEAK DETECTION TOOL — TEST SUITE${RESET}"
echo -e "${CYAN}${BOLD}════════════════════════════════════════════════════════════════${RESET}"
echo ""

PASS=0
FAIL=0

# ── run_test function ────────────────────────────────────────
# Args: test_binary  description  expected_leak_count
run_test() {
    local BIN="$1"
    local DESC="$2"
    local EXPECTED="$3"

    echo -e "${BOLD}── $DESC${RESET}"

    if [ ! -f "$BIN" ]; then
        echo -e "  ${RED}SKIP: binary not found ($BIN)${RESET}"
        echo ""
        return
    fi

    # ── Our tool ─────────────────────────────────────────────
    T_START=$(date +%s%N)
    OUTPUT=$(LD_PRELOAD=./$LIB $BIN 2>&1)
    T_END=$(date +%s%N)
    OUR_MS=$(( (T_END - T_START) / 1000000 ))

    # ── Baseline (no tool) ───────────────────────────────────
    B_START=$(date +%s%N)
    $BIN > /dev/null 2>&1
    B_END=$(date +%s%N)
    BASE_MS=$(( (B_END - B_START) / 1000000 ))

    # ── Overhead ─────────────────────────────────────────────
    if [ "$BASE_MS" -gt 0 ]; then
        OVERHEAD=$(( (OUR_MS - BASE_MS) * 100 / BASE_MS ))
    else
        OVERHEAD=0
    fi

    # ── Count leaks in our output ────────────────────────────
    OUR_LEAKS=$(echo "$OUTPUT" | grep -c "LEAK #")

    # ── Valgrind ─────────────────────────────────────────────
    VG_LEAKS="N/A"
    VG_STATUS="skipped"
    if [ "$HAVE_VALGRIND" -eq 1 ]; then
        VG_OUT=$($VALGRIND_CMD $BIN 2>&1)
        VG_LEAKS=$(echo "$VG_OUT" | grep "definitely lost" | grep -oP '\d+ bytes' | head -1)
        if [ -z "$VG_LEAKS" ]; then
            VG_LEAKS="0 bytes"
        fi
        VG_STATUS="done"
    fi

    # ── Result ───────────────────────────────────────────────
    if [ "$OUR_LEAKS" -eq "$EXPECTED" ]; then
        STATUS="${GREEN}PASS${RESET}"
        PASS=$((PASS + 1))
    else
        STATUS="${RED}FAIL (expected $EXPECTED leaks, got $OUR_LEAKS)${RESET}"
        FAIL=$((FAIL + 1))
    fi

    echo -e "  Our tool  : $OUR_LEAKS leak(s) detected      → $STATUS"
    echo -e "  Valgrind  : $VG_LEAKS ($VG_STATUS)"
    echo -e "  Timing    : baseline=${BASE_MS}ms  tool=${OUR_MS}ms  overhead=${OVERHEAD}%"
    if [ "$OVERHEAD" -gt 5 ]; then
        echo -e "  ${YELLOW}WARNING: overhead exceeds 5% target${RESET}"
    fi
    echo ""
}

# ── Run all 6 tests ──────────────────────────────────────────
run_test "./build/test1"  "Test 1 — No Leaks (expect: 0 leaks)"              0
run_test "./build/test2"  "Test 2 — Single Small Leak (expect: 1 leak)"      1
run_test "./build/test3"  "Test 3 — Multiple Leaks (expect: 10 leaks)"      10
run_test "./build/test4"  "Test 4 — Large Leak (expect: 1 leak)"             1
run_test "./build/test5"  "Test 5 — Invalid Free (expect: 1 leak + warning)" 1
run_test "./build/test6"  "Test 6 — Multi-threaded (expect: 4 leaks)"        4

# ── Summary ──────────────────────────────────────────────────
echo -e "${CYAN}${BOLD}════════════════════════════════════════════════════════════════${RESET}"
echo -e "${BOLD}  RESULTS: ${GREEN}$PASS passed${RESET}  /  ${RED}$FAIL failed${RESET}"
echo -e "${CYAN}${BOLD}════════════════════════════════════════════════════════════════${RESET}"
echo ""

exit $FAIL
