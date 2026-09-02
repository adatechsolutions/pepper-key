#!/bin/sh
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
project_dir=$(CDPATH= cd -- "$script_dir/.." && pwd)
build_dir=${TMPDIR:-/tmp}/pepper-key-core-test-$$
trap 'rm -rf "$build_dir"' EXIT HUP INT TERM
mkdir -p "$build_dir"

openssl_cflags=
openssl_ldflags=-lcrypto
if [ "$(uname -s)" = Darwin ] && [ -d /opt/homebrew/opt/openssl@3 ]; then
    openssl_cflags=-I/opt/homebrew/opt/openssl@3/include
    openssl_ldflags="-L/opt/homebrew/opt/openssl@3/lib -lcrypto"
elif command -v pkg-config >/dev/null 2>&1 && pkg-config --exists openssl; then
    openssl_cflags=$(pkg-config --cflags openssl)
    openssl_ldflags=$(pkg-config --libs openssl)
elif ! printf '#include <openssl/hmac.h>\n' | cc -E - >/dev/null 2>&1; then
    printf '%s\n' 'PepperKey core tests held: OpenSSL development headers are unavailable.' >&2
    exit 77
fi

# shellcheck disable=SC2086
cc -std=c11 -Wall -Wextra -Werror \
    $openssl_cflags \
    -I"$project_dir/app" \
    "$project_dir/app/pepper_secure.c" \
    "$project_dir/app/pepper_base32.c" \
    "$project_dir/app/pepper_otp.c" \
    "$project_dir/app/pepper_vault.c" \
    "$project_dir/app/pepper_challenge.c" \
    "$project_dir/app/pepper_ctap2.c" \
    "$project_dir/tests/native/pepper_crypto_openssl.c" \
    "$project_dir/tests/native/test_core.c" \
    $openssl_ldflags \
    -o "$build_dir/test_core"

"$build_dir/test_core"

python3 "$project_dir/tests/check-capabilities.py"
