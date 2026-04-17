#!/bin/bash

echo "════════════════════════════════════════════════════════════════"
echo "   MEMORY LEAK DETECTION TOOL — TEST SUITE"
echo "════════════════════════════════════════════════════════════════"
echo ""

TESTS=("test1" "test2" "test3" "test4" "test5" "test6")

for TEST_NAME in "${TESTS[@]}"; do
    echo "── $TEST_NAME ──────────────────────────────────────────"

    # Define permanent log file paths for this specific test
    VALGRIND_LOG="logs/${TEST_NAME}_valgrind.log"
    TOOL_LOG="logs/${TEST_NAME}_memtrack.txt"

    # 1. Run Valgrind on the PURE binary, saving to its own permanent log file
    valgrind --leak-check=full --log-file=$VALGRIND_LOG ./build/${TEST_NAME}_valgrind > /dev/null 2>&1
    
    # Extract the 'definitely lost' bytes from Valgrind log (Column 4)
    VALGRIND_BYTES=$(grep "definitely lost:" $VALGRIND_LOG | awk '{print $4}')
    if [ -z "$VALGRIND_BYTES" ]; then VALGRIND_BYTES="0"; fi

    # Extract the 'definitely lost' blocks from Valgrind log (Column 7)
    VALGRIND_BLOCKS=$(grep "definitely lost:" $VALGRIND_LOG | awk '{print $7}')
    if [ -z "$VALGRIND_BLOCKS" ]; then VALGRIND_BLOCKS="0"; fi

    # 2. Run your tool on the LINKED binary, explicitly telling it to generate a log file
    TOOL_OUTPUT=$(MEMTRACK_LOG=$TOOL_LOG LD_LIBRARY_PATH=./build ./build/$TEST_NAME 2>&1)
    
    # Extract the 'Leaked blocks' count
    TOOL_LEAKS=$(echo "$TOOL_OUTPUT" | grep "Leaked blocks" | awk -F':' '{print $2}' | tr -d ' ')
    if [ -z "$TOOL_LEAKS" ]; then TOOL_LEAKS="0"; fi

    # Extract the 'Leaked bytes' string
    TOOL_BYTES=$(echo "$TOOL_OUTPUT" | grep "Leaked bytes" | awk -F':' '{print $2}' | xargs)
    if [ -z "$TOOL_BYTES" ]; then TOOL_BYTES="0 bytes"; fi

    # 3. Print the perfectly mirrored comparison
    echo "  Our tool  : $TOOL_LEAKS block(s) leaked ($TOOL_BYTES)"
    echo "  Valgrind  : $VALGRIND_BLOCKS block(s) leaked ($VALGRIND_BYTES bytes)"
    echo ""

done

echo "════════════════════════════════════════════════════════════════"
echo "   TESTING COMPLETE. Check the logs/ directory for details."
echo "════════════════════════════════════════════════════════════════"