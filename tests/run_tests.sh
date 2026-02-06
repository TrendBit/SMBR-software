#!/bin/bash
set -e

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"

# 1) Run pytest
echo "=== Running pytest ==="
pytest -v "$ROOT_DIR" || echo "Pytest finished with failures, continuing..."

# 2) Load BASE_URL from config.py
BASE_URL=$(python3 "$ROOT_DIR/config.py")
echo "Using BASE_URL: $BASE_URL"

# 3) Paths to schemathesis configuration
OPERATIONS_FILE="$ROOT_DIR/schemathesis/operations.txt"

# 4) Load operation-id
ARGS=""
while IFS= read -r op; do
  [[ -z "$op" || "$op" =~ ^# ]] && continue
  ARGS="$ARGS --include-operation-id $op"
done < "$OPERATIONS_FILE"

# 5) Run Schemathesis
echo "=== Running Schemathesis ==="
uvx schemathesis run "$BASE_URL/api-docs/oas-3.0.0.json" $ARGS
