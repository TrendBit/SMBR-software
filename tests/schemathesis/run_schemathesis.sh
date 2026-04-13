#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

# Load BASE_URL from config.py
BASE_URL=$(python3 "$ROOT_DIR/config.py")
echo "Using BASE_URL: $BASE_URL"

# Paths to schemathesis configuration
OPERATIONS_FILE="$SCRIPT_DIR/operations.txt"

# Build --include-operation-id arguments
ARGS=""
while IFS= read -r op; do
  [[ -z "$op" || "$op" =~ ^# ]] && continue
  ARGS="$ARGS --include-operation-id $op"
done < "$OPERATIONS_FILE"

# Run Schemathesis
echo "=== Running Schemathesis ==="
uvx schemathesis run "$BASE_URL/api-docs/oas-3.0.0.json" $ARGS
