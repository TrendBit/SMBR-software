#!/bin/bash

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"

UNIT_FAILED=0
BEHAVIORAL_FAILED=0
SCHEMATHESIS_FAILED=0

# 1) Unit tests
echo "=== Running unit tests ==="
pytest -v "$ROOT_DIR/unit" || UNIT_FAILED=1

# 2) Behavioral tests
echo "=== Running behavioral tests ==="
pytest -v "$ROOT_DIR/behavioral" || BEHAVIORAL_FAILED=1

# 3) Schemathesis
echo "=== Running Schemathesis ==="
"$ROOT_DIR/schemathesis/run_schemathesis.sh" || SCHEMATHESIS_FAILED=1

# Summary
echo ""
echo "=== Summary ==="
[[ $UNIT_FAILED -eq 0 ]]        && echo "Unit tests:        PASSED" || echo "Unit tests:        FAILED"
[[ $BEHAVIORAL_FAILED -eq 0 ]]  && echo "Behavioral tests:  PASSED" || echo "Behavioral tests:  FAILED"
[[ $SCHEMATHESIS_FAILED -eq 0 ]] && echo "Schemathesis:      PASSED" || echo "Schemathesis:      FAILED"

# Exit with non-zero if any suite failed
[[ $UNIT_FAILED -eq 0 && $BEHAVIORAL_FAILED -eq 0 && $SCHEMATHESIS_FAILED -eq 0 ]]
