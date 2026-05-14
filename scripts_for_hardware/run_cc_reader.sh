           #!/usr/bin/env bash
set -euo pipefail

# Force the use of system libraries and ignore Snap-related overrides
export LD_LIBRARY_PATH="/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu"
# If you have built the middleware libraries, add them too:
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:$(pwd)/../pteid-mw-pt/_src/eidmw/lib"
# Prevent Snap from injecting its own libraries
unset LD_PRELOAD

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
DOCS_DIR="${SCRIPT_DIR}/docs_for_signing"
CERTS_CACHE_DIR="${HOME}/.pteid-ng/certs"
MW_CERTS_DIR="${SCRIPT_DIR}/../pteid-mw-pt/_src/eidmw/misc/certs"

mkdir -p "${DOCS_DIR}/signed"
mkdir -p "${CERTS_CACHE_DIR}"

if [[ ! -x "/usr/local/bin/pteiddialogsQTsrv" ]]; then
    echo "WARNING: /usr/local/bin/pteiddialogsQTsrv not found or is not executable."
    echo "The SDK requires this dialog server to complete document signing."
    echo "Please run 'sudo make install' in your pteid-mw-pt/_src/eidmw build directory,"
    echo "or manually symlink the executable to /usr/local/bin/."
fi

if [[ ! -f "${CERTS_CACHE_DIR}/cacerts.pem" && -f "${MW_CERTS_DIR}/cacerts.pem" ]]; then
    cp "${MW_CERTS_DIR}/cacerts.pem" "${CERTS_CACHE_DIR}/cacerts.pem"
fi

if [[ -z "${EID_SIGN_PIN:-}" ]]; then
    read -rsp "Enter Signature PIN: " EID_SIGN_PIN
    echo
    export EID_SIGN_PIN
fi

if [ -d "${BUILD_DIR}" ] && [ -f "${BUILD_DIR}/CMakeCache.txt" ]; then
    echo "Removing stale build directory: ${BUILD_DIR}"
    rm -rf "${BUILD_DIR}"
fi

cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}"
cmake --build "${BUILD_DIR}" -j

# Sanitize environment and run the reader with integrated signer
env -i HOME="$HOME" USER="$USER" PATH="/usr/local/bin:/usr/bin:/bin" \
    LD_LIBRARY_PATH="/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu:${SCRIPT_DIR}/../pteid-mw-pt/_src/eidmw/lib" \
    EID_SIGN_PIN="$EID_SIGN_PIN" \
    "${BUILD_DIR}/cc_reader" "${DOCS_DIR}"
