#!/usr/bin/env bash
# Formats every Lumine C++ source file in place with clang-format.
set -euo pipefail

cd "$(dirname "$0")/.."

if ! command -v clang-format >/dev/null 2>&1; then
    echo "error: clang-format not found (install the 'clang' package)" >&2
    exit 1
fi

mapfile -t files < <(find src tests -type f \( -name '*.cpp' -o -name '*.hpp' \))

if [[ ${#files[@]} -eq 0 ]]; then
    echo "No C++ sources found."
    exit 0
fi

clang-format -i "${files[@]}"
echo "Formatted ${#files[@]} file(s)."
