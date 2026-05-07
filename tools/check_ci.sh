#!/usr/bin/env bash
# Run local quality gates matching CI pipeline checks.
# Performs: clang-format, clang-tidy, CMake build, unit tests, security checks, coverage.
# Proposes fix commands if any checks fail.
set -euo pipefail

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

MODE="check"
TOTAL=0
FAILED=0
FAILED_CHECKS=()
SKIP_DOCKER=false
BUILD_DIR="build"

usage() {
  cat <<'EOF'
Usage: tools/check_ci.sh [OPTIONS]

Options:
  --fix           Apply auto-fixes (clang-format) before checks (default: check-only)
  --check-only    Run checks only, no modifications (default)
  --skip-docker   Skip Docker build check
  --build-dir DIR Use alternative build directory (default: build/)
  --help          Show this help message
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --fix)
      MODE="fix"
      shift
      ;;
    --check-only)
      MODE="check"
      shift
      ;;
    --skip-docker)
      SKIP_DOCKER=true
      shift
      ;;
    --build-dir)
      BUILD_DIR="$2"
      shift 2
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      echo -e "${RED}Unknown argument: $1${NC}"
      usage
      exit 2
      ;;
  esac
done

require_cmd() {
  if ! command -v "$1" >/dev/null 2>&1; then
    echo -e "${RED}✗ Missing required command: $1${NC}"
    exit 2
  fi
}

run_step() {
  local title="$1"
  local cmd="$2"
  TOTAL=$((TOTAL + 1))

  echo -e "\n${BLUE}== ${title} ==${NC}"
  if eval "$cmd" 2>&1; then
    echo -e "${GREEN}✓ PASS${NC}"
  else
    echo -e "${RED}✗ FAIL${NC}"
    FAILED=$((FAILED + 1))
    FAILED_CHECKS+=("$title")
  fi
}

run_step_nonzero() {
  local title="$1"
  local cmd="$2"
  TOTAL=$((TOTAL + 1))

  echo -e "\n${BLUE}== ${title} ==${NC}"
  # Allow non-zero exit; capture output
  if eval "$cmd" 2>&1; then
    echo -e "${GREEN}✓ PASS${NC}"
  else
    echo -e "${YELLOW}⚠ REVIEW (non-blocking)${NC}"
  fi
}

# Verify required commands are available
echo -e "${BLUE}Checking required tools...${NC}"
require_cmd cmake
require_cmd make
require_cmd clang-format
require_cmd clang-tidy
require_cmd ctest

echo -e "${GREEN}All required tools found${NC}"
echo -e "${BLUE}Running local CI checks (mode: ${MODE})${NC}\n"

# ============================================================================
# LINT CHECKS
# ============================================================================

if [[ "$MODE" == "fix" ]]; then
  echo -e "${YELLOW}Applying clang-format fixes...${NC}"
  find src tests -type f \( -name "*.cpp" -o -name "*.h" \) -print0 2>/dev/null | \
    xargs -0 clang-format -i 2>/dev/null || true
  echo -e "${GREEN}✓ Auto-formatting applied${NC}"
fi

run_step "clang-format code formatting" \
  "find src tests -type f \( -name '*.cpp' -o -name '*.h' \) -print0 2>/dev/null | xargs -0 clang-format --dry-run --Werror"

run_step "CMake configuration (compile_commands.json)" \
  "rm -rf '$BUILD_DIR' && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ."

run_step_nonzero "clang-tidy static analysis" \
  "find src -type f -name '*.cpp' -print0 2>/dev/null | xargs -0 clang-tidy -p '$BUILD_DIR' 2>&1"

# ============================================================================
# SECURITY CHECKS
# ============================================================================

run_step "Security: hardcoded secrets check" \
  "! grep -rn --include='*.cpp' --include='*.h' -iE '(password|secret|api_key)\\s*=\\s*\"[^\"]+\"' src/ 2>/dev/null || (echo 'Hardcoded secret detected'; exit 1)"

run_step "Security: CMake unsafe shell invocations" \
  "! grep -iE 'execute_process|system\\(' CMakeLists.txt src/CMakeLists.txt 2>/dev/null || (echo '::warning::Shell execution found in CMake — review manually'; exit 0)"

# ============================================================================
# BUILD & TEST
# ============================================================================

run_step "CMake configuration (coverage build)" \
  "rm -rf '$BUILD_DIR' && cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS='--coverage' ."

run_step "Build" \
  "make -j\$(nproc) 2>&1"

run_step "Unit tests (ctest)" \
  "ctest --output-on-failure"

run_step "Coverage report generation" \
  "lcov --directory . --capture --output-file coverage.info >/dev/null && lcov --remove coverage.info '/usr/*' --output-file coverage.info >/dev/null && lcov --list coverage.info | head -20"

# ============================================================================
# DOCKER SMOKE TEST
# ============================================================================

if [[ "$SKIP_DOCKER" == false ]]; then
  run_step "Docker build (smoke test)" \
    "docker build -t medtech-clinician-ui:ci . 2>&1"
fi

# ============================================================================
# SUMMARY & FIX SUGGESTIONS
# ============================================================================

echo -e "\n${BLUE}═══════════════════════════════════════════════════${NC}"
echo -e "${BLUE}Summary: $((TOTAL - FAILED))/${TOTAL} checks passed${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════${NC}\n"

if [[ $FAILED -ne 0 ]]; then
  echo -e "${RED}❌ ${FAILED} check(s) failed:${NC}"
  for check in "${FAILED_CHECKS[@]}"; do
    echo -e "  ${RED}•${NC} $check"
  done

  echo -e "\n${YELLOW}Suggested fix commands:${NC}\n"

  for check in "${FAILED_CHECKS[@]}"; do
    case "$check" in
      "clang-format code formatting")
        echo -e "${CYAN}Fix formatting issues:${NC}"
        echo -e "  ${CYAN}find src tests -type f \\( -name '*.cpp' -o -name '*.h' \\) -print0 | xargs -0 clang-format -i${NC}\n"
        ;;
      "clang-tidy static analysis")
        echo -e "${CYAN}Review and fix clang-tidy warnings:${NC}"
        echo -e "  ${CYAN}find src -name '*.cpp' -print0 | xargs -0 clang-tidy -p build --fix${NC}\n"
        ;;
      "CMake configuration"*)
        echo -e "${CYAN}Clean and reconfigure CMake:${NC}"
        echo -e "  ${CYAN}rm -rf build && cmake .${NC}\n"
        ;;
      "Build")
        echo -e "${CYAN}Check build errors and fix compilation issues:${NC}"
        echo -e "  ${CYAN}make -j\$(nproc)${NC}\n"
        ;;
      "Unit tests"*)
        echo -e "${CYAN}Run tests with verbose output:${NC}"
        echo -e "  ${CYAN}ctest --output-on-failure -V${NC}\n"
        ;;
      "Security: hardcoded secrets check")
        echo -e "${CYAN}Review and remove hardcoded secrets from code:${NC}"
        echo -e "  ${CYAN}grep -rn --include='*.cpp' --include='*.h' -iE '(password|secret|api_key)\\s*=\\s*\"[^\"]+\"' src/${NC}\n"
        ;;
      "Security: CMake unsafe shell invocations")
        echo -e "${CYAN}Review and secure shell invocations in CMake:${NC}"
        echo -e "  ${CYAN}grep -n -iE 'execute_process|system\\(' CMakeLists.txt src/CMakeLists.txt${NC}\n"
        ;;
      "Coverage report generation")
        echo -e "${CYAN}Generate coverage report manually:${NC}"
        echo -e "  ${CYAN}lcov --directory . --capture --output-file coverage.info${NC}"
        echo -e "  ${CYAN}lcov --remove coverage.info '/usr/*' --output-file coverage.info${NC}"
        echo -e "  ${CYAN}lcov --list coverage.info${NC}\n"
        ;;
      "Docker build (smoke test)")
        echo -e "${CYAN}Check Dockerfile and build issues:${NC}"
        echo -e "  ${CYAN}docker build -t medtech-clinician-ui:ci . --progress=plain${NC}\n"
        ;;
    esac
  done

  exit 1
fi

echo -e "${GREEN}✓ All checks passed!${NC}"
exit 0