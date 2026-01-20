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

ENDPOINT="${BASE_URL}/sensor/oled/print_custom_text"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Testing: POST /sensor/oled/print_custom_text${NC}"
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
test_request "Regular text" '{"text": "Hello World"}' "200"
test_request "Empty string" '{"text": ""}' "200"
test_request "Text with spaces" '{"text": "   text with spaces   "}' "200"
test_request "Special characters" '{"text": "Test123!@#$%"}' "200"
test_request "Unicode characters" '{"text": "Příliš žluťoučký kůň"}' "200"
test_request "Text with newlines" '{"text": "Line1\nLine2"}' "200"
test_request "Long text" '{"text": "This is a very long text that might overflow the display but should still be accepted by the API"}' "200"

echo -e "${BLUE}Testing Error Cases - ErrorHandler (Expected: 400)${NC}\n"
test_request "Empty body" '{}' "400"
test_request "Missing text field" '{"message": "hello"}' "400"
test_request "Null instead of string" '{"text": null}' "400"
test_request "Number instead of string" '{"text": 123}' "400"
test_request "Boolean instead of string" '{"text": true}' "400"
test_request "Array instead of string" '{"text": ["hello", "world"]}' "400"
test_request "Object instead of string" '{"text": {"message": "hello"}}' "400"
test_request "Invalid JSON (unclosed bracket)" '{"text": "hello"' "400"
test_request "Invalid JSON (missing quotes)" '{text: hello}' "400"

echo -e "${YELLOW}Framework Limitations (Expected: 200 OK)${NC}\n"
test_request "Additional property (simple)" '{"text": "hello", "extra": "value"}' "200"
test_request "Additional property (complex)" '{"text": "hello", "extra": {"nested": true}}' "200"
test_request "Duplicate keys (last value used)" '{"text": "first", "text": "second"}' "200"
test_request "Trailing comma (accepted by oatpp)" '{"text": "hello",}' "200"

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
