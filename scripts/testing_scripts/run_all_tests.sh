#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Counter for test suites
TOTAL_SUITES=0
PASSED_SUITES=0
FAILED_SUITES=0

# Function to run a test script
run_test_suite() {
    local script_name="$1"
    local script_path="$SCRIPT_DIR/$script_name"
    
    TOTAL_SUITES=$((TOTAL_SUITES + 1))
    
    echo -e "${BLUE}═══════════════════════════════════════════════${NC}"
    echo -e "${BLUE}Running Test Suite: $script_name${NC}"
    echo -e "${BLUE}═══════════════════════════════════════════════${NC}\n"
    
    if [ -f "$script_path" ]; then
        bash "$script_path"
        exit_code=$?
        
        if [ $exit_code -eq 0 ]; then
            echo -e "${GREEN}Test suite PASSED: $script_name${NC}\n"
            PASSED_SUITES=$((PASSED_SUITES + 1))
        else
            echo -e "${RED}Test suite FAILED: $script_name${NC}\n"
            FAILED_SUITES=$((FAILED_SUITES + 1))
        fi
    else
        echo -e "${RED}Test script not found: $script_path${NC}\n"
        FAILED_SUITES=$((FAILED_SUITES + 1))
    fi
}

# Run all test scripts
run_test_suite "test_printCustomText.sh"
#run_test_suite "test_setIntensities.sh"

# Print final summary
echo -e "${CYAN}║════════════Overall Test Summary════════════║${NC}"

echo -e "Total Test Suites:  $TOTAL_SUITES"
echo -e "${GREEN}Passed:             $PASSED_SUITES${NC}"
echo -e "${RED}Failed:             $FAILED_SUITES${NC}"
echo -e "${CYAN}════════════════════════════════════════════════${NC}\n"

# Exit with error code if any test suite failed
if [ $FAILED_SUITES -gt 0 ]; then
    echo -e "${RED}Some test suites failed. Please review the output above.${NC}"
    exit 1
fi

echo -e "${GREEN}All test suites passed successfully!${NC}"
exit 0
