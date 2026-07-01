#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"
GENERATOR="Visual Studio 17 2022"
CONFIG="Release"
POLICY_MIN="3.5"

DO_CONFIGURE=0
DO_BUILD=1
DO_RUN=1
DO_CLEAN=0

usage() {
    cat <<EOF
Usage: ./scripts/dev.sh [options]

Options:
  --configure      Force CMake configure step
  --build-only     Configure/build only, do not run
  --run-only       Run existing binary only
  --debug          Build/run Debug config
  --release        Build/run Release config (default)
  --clean          Delete build directory before running
  -h, --help       Show this help

Examples:
  ./scripts/dev.sh
  ./scripts/dev.sh --configure
  ./scripts/dev.sh --debug
  ./scripts/dev.sh --clean --configure
  ./scripts/dev.sh --run-only
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --configure)
            DO_CONFIGURE=1
            shift
            ;;
        --build-only)
            DO_RUN=0
            shift
            ;;
        --run-only)
            DO_BUILD=0
            DO_RUN=1
            shift
            ;;
        --debug)
            CONFIG="Debug"
            shift
            ;;
        --release)
            CONFIG="Release"
            shift
            ;;
        --clean)
            DO_CLEAN=1
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

if [[ "$DO_CLEAN" -eq 1 ]]; then
    echo "[dev] Cleaning build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi

if [[ "$DO_BUILD" -eq 1 ]]; then
    if [[ ! -d "$BUILD_DIR" ]]; then
        DO_CONFIGURE=1
    fi

    if [[ "$DO_CONFIGURE" -eq 1 ]]; then
        echo "[dev] Configuring project ($CONFIG)"
        cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -G "$GENERATOR" -DCMAKE_POLICY_VERSION_MINIMUM="$POLICY_MIN"
    fi

    echo "[dev] Building project ($CONFIG)"
    cmake --build "$BUILD_DIR" --config "$CONFIG"
fi

if [[ "$DO_RUN" -eq 1 ]]; then
    EXE_PATH="$BUILD_DIR/bin/$CONFIG/game.exe"

    if [[ ! -f "$EXE_PATH" ]]; then
        echo "[dev] Binary not found: $EXE_PATH"
        echo "[dev] Try: ./scripts/dev.sh --configure"
        exit 1
    fi

    echo "[dev] Running: $EXE_PATH"
    "$EXE_PATH"
fi
