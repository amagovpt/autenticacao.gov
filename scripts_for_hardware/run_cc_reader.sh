           #!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

if [ -d "${BUILD_DIR}" ] && [ -f "${BUILD_DIR}/CMakeCache.txt" ]; then
    echo "Removing stale build directory: ${BUILD_DIR}"
    rm -rf "${BUILD_DIR}"
fi

cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}"
cmake --build "${BUILD_DIR}" -j
"${BUILD_DIR}/cc_reader"
