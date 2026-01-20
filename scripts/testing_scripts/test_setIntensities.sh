#!/bin/bash

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Load configuration
source "$SCRIPT_DIR/test_config.sh"

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

ENDPOINT="${BASE_URL}/control/led_panel/intensity"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Testing: POST /control/led_panel/intensity${NC}"
echo -e "${BLUE}========================================${NC}\n"

# Counter for tests
TOTAL=0
PASSED=0
FAILED=0

# Function to test and check response
test_request() {
    local description="$1"
    local data="$2"
    local expected_status="$3"
    
    TOTAL=$((TOTAL + 1))
    echo -e "${YELLOW}Test $TOTAL:${NC} $description"
    
    response=$(curl -s -w "\n%{http_code}" -X POST -H 'Content-Type: application/json' -d "$data" "$ENDPOINT")
    http_code=$(echo "$response" | tail -n1)
    body=$(echo "$response" | head -n-1)
    
    if [ "$http_code" = "$expected_status" ]; then
        echo -e "${GREEN}PASS${NC} (HTTP $http_code)"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}FAIL${NC} (Expected: $expected_status, Got: $http_code)"
        echo -e "Response: $body"
        FAILED=$((FAILED + 1))
    fi
    echo ""
}

echo -e "${GREEN}Valid Requests (Expected: 200 OK)${NC}\n"
test_request "Correct format" '{"intensity": [0.5, 1, 0, 0.2]}' "200"
test_request "All zeros" '{"intensity": [0, 0, 0, 0]}' "200"
test_request "All ones" '{"intensity": [1.0, 1.0, 1.0, 1.0]}' "200"
test_request "Decimal values" '{"intensity": [0.123, 0.456, 0.789, 0.999]}' "200"

echo -e "${RED}Errors Caught by ErrorHandler (Expected: 400)${NC}\n"
test_request "Empty body" '{}' "400"
test_request "Missing intensity field" '{"other": [1,2,3,4]}' "400"
test_request "Null instead of array" '{"intensity": null}' "400"
test_request "String instead of array" '{"intensity": "text"}' "400"
test_request "Object instead of array" '{"intensity": {"a": 1}}' "400"
test_request "Number instead of array" '{"intensity": 123}' "400"
test_request "Wrong type in array - string" '{"intensity": [0.5, "text", 0, 0.2]}' "400"
test_request "Wrong type in array - object" '{"intensity": [0.5, {"a":1}, 0, 0.2]}' "400"
test_request "Invalid JSON" '{invalid json}' "400"
test_request "Empty array" '{"intensity": []}' "400"
test_request "Only 1 element" '{"intensity": [0.5]}' "400"
test_request "Only 2 elements" '{"intensity": [0.5, 1]}' "400"
test_request "Only 3 elements" '{"intensity": [0.5, 1, 0]}' "400"
test_request "5 elements" '{"intensity": [0.5, 1, 0, 0.2, 0.3]}' "400"

echo -e "${RED}Input Validation Errors (Expected: 500)${NC}\n"
test_request "Null element in array" '{"intensity": [null, 1, 0, 0.2]}' "500"
test_request "All nulls" '{"intensity": [null, null, null, null]}' "500"
test_request "Value out of range - negative" '{"intensity": [-0.1, 1, 0, 0.2]}' "500"
test_request "Value out of range - >1" '{"intensity": [0.5, 1.1, 0, 0.2]}' "500"
test_request "All values out of range" '{"intensity": [-1, 2, -0.5, 1.5]}' "500"

echo -e "${YELLOW}Framework Limitations (Expected: 200 OK)${NC}\n"
test_request "Additional property (simple)" '{"intensity": [0.5,1,0,0.2], "extra": "value"}' "200"
test_request "Additional property (complex)" '{"intensity": [0.5,1,0,0.2], "extra": {"nested": true}}' "200"
test_request "Duplicate keys" '{"intensity": [0,0,0,0], "intensity": [1,1,1,1]}' "200"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Test Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Total:  $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo -e "${BLUE}========================================${NC}\n"

# Exit with error code if any test failed
if [ $FAILED -gt 0 ]; then
    exit 1
fi

exit 0
