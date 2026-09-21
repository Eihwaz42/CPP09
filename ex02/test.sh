#!/bin/bash

PROGRAM="./PmergeMe"
PASS=0
FAIL=0

GREEN="\033[32m"
RED="\033[31m"
YELLOW="\033[33m"
RESET="\033[0m"

pass()
{
    echo -e "${GREEN}[OK]${RESET} $1"
    PASS=$((PASS + 1))
}

fail()
{
    echo -e "${RED}[FAIL]${RESET} $1"
    FAIL=$((FAIL + 1))
}

# ============================================================
# Build
# ============================================================

echo "========================================"
echo "              BUILD"
echo "========================================"

make fclean > /dev/null 2>&1
make > /dev/null 2>&1

if [ $? -ne 0 ]; then
    fail "Compilation"
    echo "Compilation failed. Stopping tests."
    exit 1
fi

pass "Compilation"

# ============================================================
# Valid input checker
# ============================================================

check_valid()
{
    NAME="$1"
    shift

    EXPECTED=$(printf "%s\n" "$@" | awk '{print $1 + 0}' | sort -n | paste -sd ' ' -)

    STDOUT_FILE=$(mktemp)
    STDERR_FILE=$(mktemp)

    "$PROGRAM" "$@" > "$STDOUT_FILE" 2> "$STDERR_FILE"
    STATUS=$?

    if [ $STATUS -ne 0 ]; then
        fail "$NAME (program returned $STATUS)"
        rm -f "$STDOUT_FILE" "$STDERR_FILE"
        return
    fi

    if [ -s "$STDERR_FILE" ]; then
        fail "$NAME (unexpected stderr)"
        rm -f "$STDOUT_FILE" "$STDERR_FILE"
        return
    fi

    AFTER=$(grep '^After:' "$STDOUT_FILE" | sed 's/^After:[[:space:]]*//')

    AFTER_COUNT=$(grep -c '^After:' "$STDOUT_FILE")

    if [ "$AFTER_COUNT" -ne 1 ]; then
        fail "$NAME (expected exactly one After line)"
        rm -f "$STDOUT_FILE" "$STDERR_FILE"
        return
    fi

    if [ "$AFTER" != "$EXPECTED" ]; then
        fail "$NAME (wrong sorted result)"
        echo "       Expected: $EXPECTED"
        echo "       Got:      $AFTER"
        rm -f "$STDOUT_FILE" "$STDERR_FILE"
        return
    fi

    VECTOR_TIME=$(grep -c 'std::vector' "$STDOUT_FILE")
    DEQUE_TIME=$(grep -c 'std::deque' "$STDOUT_FILE")

    if [ "$VECTOR_TIME" -ne 1 ] || [ "$DEQUE_TIME" -ne 1 ]; then
        fail "$NAME (missing/duplicate timing line)"
        rm -f "$STDOUT_FILE" "$STDERR_FILE"
        return
    fi

    pass "$NAME"

    rm -f "$STDOUT_FILE" "$STDERR_FILE"
}

# ============================================================
# Invalid input checker
# ============================================================

check_invalid()
{
    NAME="$1"
    shift

    STDOUT_FILE=$(mktemp)
    STDERR_FILE=$(mktemp)

    "$PROGRAM" "$@" > "$STDOUT_FILE" 2> "$STDERR_FILE"
    STATUS=$?

    if [ $STATUS -eq 0 ]; then
        fail "$NAME (returned success)"
        rm -f "$STDOUT_FILE" "$STDERR_FILE"
        return
    fi

    if [ -s "$STDOUT_FILE" ]; then
        fail "$NAME (wrote error/output to stdout)"
        rm -f "$STDOUT_FILE" "$STDERR_FILE"
        return
    fi

    if [ ! -s "$STDERR_FILE" ]; then
        fail "$NAME (nothing written to stderr)"
        rm -f "$STDOUT_FILE" "$STDERR_FILE"
        return
    fi

    pass "$NAME"

    rm -f "$STDOUT_FILE" "$STDERR_FILE"
}

# ============================================================
# Fixed valid tests
# ============================================================

echo
echo "========================================"
echo "          FIXED VALID TESTS"
echo "========================================"

check_valid "Single element" 42
check_valid "Two elements" 5 4
check_valid "Already sorted" 1 2 3 4 5
check_valid "Reverse sorted" 5 4 3 2 1
check_valid "Odd count" 9 3 7 1 8
check_valid "Even count" 8 3 5 2 9 1
check_valid "Duplicates" 8 3 8 2 8 1
check_valid "Many duplicates" 5 5 5 5 5 5 5
check_valid "INT_MAX" 2147483647
check_valid "Mixed with INT_MAX" 42 2147483647 1 999 36
check_valid "Leading zeros" 00042 003 0001

# ============================================================
# Invalid tests
# ============================================================

echo
echo "========================================"
echo "            INVALID TESTS"
echo "========================================"

check_invalid "No arguments"
check_invalid "Zero" 0
check_invalid "Negative" -1
check_invalid "Negative in sequence" 42 -3 8
check_invalid "Letters" patate
check_invalid "Letters after number" 12patate
check_invalid "Letters before number" patate12
check_invalid "Plus sign" +42
check_invalid "Decimal" 4.2
check_invalid "INT_MAX + 1" 2147483648
check_invalid "Huge integer" 999999999999999999999999999999999999
check_invalid "Space inside argument" "42 36"
check_invalid "Empty argument" ""

# ============================================================
# Random tests
# ============================================================

echo
echo "========================================"
echo "             RANDOM TESTS"
echo "========================================"

random_test()
{
    COUNT="$1"
    MAX="$2"
    RUNS="$3"

    for ((run = 1; run <= RUNS; run++)); do

        mapfile -t ARGS < <(shuf -i 1-"$MAX" -n "$COUNT")

        EXPECTED=$(printf "%s\n" "${ARGS[@]}" | sort -n | paste -sd ' ' -)

        OUTPUT=$("$PROGRAM" "${ARGS[@]}" 2>/dev/null)
        STATUS=$?

        if [ $STATUS -ne 0 ]; then
            fail "Random $COUNT elements, run $run (exit $STATUS)"
            return
        fi

        AFTER=$(echo "$OUTPUT" | grep '^After:' | sed 's/^After:[[:space:]]*//')

        if [ "$AFTER" != "$EXPECTED" ]; then
            fail "Random $COUNT elements, run $run"
            echo "Input:    ${ARGS[*]}"
            echo "Expected: $EXPECTED"
            echo "Got:      $AFTER"
            return
        fi
    done

    pass "$RUNS random tests with $COUNT elements"
}

random_test 10 100000 50
random_test 21 100000 50
random_test 100 100000 20
random_test 501 100000 10
random_test 1000 100000 5
random_test 3000 100000 3

# ============================================================
# Duplicate stress tests
# ============================================================

echo
echo "========================================"
echo "          DUPLICATE STRESS"
echo "========================================"

DUPLICATE_FAIL=0

for ((run = 1; run <= 20; run++)); do

    ARGS=()

    for ((i = 0; i < 200; i++)); do
        ARGS+=($((RANDOM % 10 + 1)))
    done

    EXPECTED=$(printf "%s\n" "${ARGS[@]}" | sort -n | paste -sd ' ' -)

    OUTPUT=$("$PROGRAM" "${ARGS[@]}" 2>/dev/null)
    STATUS=$?

    AFTER=$(echo "$OUTPUT" | grep '^After:' | sed 's/^After:[[:space:]]*//')

    if [ $STATUS -ne 0 ] || [ "$AFTER" != "$EXPECTED" ]; then
        fail "Duplicate stress, run $run"
        echo "Input:    ${ARGS[*]}"
        echo "Expected: $EXPECTED"
        echo "Got:      $AFTER"
        DUPLICATE_FAIL=1
        break
    fi

done

if [ $DUPLICATE_FAIL -eq 0 ]; then
    pass "20 duplicate-heavy tests"
fi

# ============================================================
# Valgrind
# ============================================================

echo
echo "========================================"
echo "              VALGRIND"
echo "========================================"

if command -v valgrind > /dev/null 2>&1; then

    VALGRIND_OUTPUT=$(mktemp)

    valgrind \
        --leak-check=full \
        --show-leak-kinds=all \
        --errors-for-leak-kinds=all \
        --error-exitcode=42 \
        "$PROGRAM" 8 3 5 2 9 1 7 4 6 \
        > /dev/null 2> "$VALGRIND_OUTPUT"

    STATUS=$?

    if [ $STATUS -eq 0 ]; then
        pass "Valgrind"
    elif [ $STATUS -eq 42 ]; then
        fail "Valgrind (memory error detected)"
    else
        fail "Valgrind (program exited with status $STATUS)"
    fi

    rm -f "$VALGRIND_OUTPUT"

else
    echo -e "${YELLOW}[SKIP]${RESET} Valgrind not installed"
fi

# ============================================================
# Final result
# ============================================================

echo
echo "========================================"
echo "              RESULTS"
echo "========================================"

echo -e "${GREEN}PASS:${RESET} $PASS"
echo -e "${RED}FAIL:${RESET} $FAIL"

if [ $FAIL -eq 0 ]; then
    echo
    echo -e "${GREEN}ALL TESTS PASSED${RESET}"
    exit 0
else
    echo
    echo -e "${RED}SOME TESTS FAILED${RESET}"
    exit 1
fi