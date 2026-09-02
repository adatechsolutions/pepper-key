#!/bin/sh
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
project_dir=$(CDPATH= cd -- "$script_dir/.." && pwd)
app_dir=$project_dir/app

require_text() {
    needle=$1
    file=$2
    if ! grep -Fq -- "$needle" "$file"; then
        printf 'missing required text: %s in %s\n' "$needle" "$file" >&2
        exit 1
    fi
}

forbid_text() {
    needle=$1
    file=$2
    if grep -Fq -- "$needle" "$file"; then
        printf 'forbidden text present: %s in %s\n' "$needle" "$file" >&2
        exit 1
    fi
}

require_text 'appid="pepper_key"' "$app_dir/application.fam"
require_text 'fap_author="Ada Tech Solutions Inc."' "$app_dir/application.fam"
require_text 'apptype=FlipperAppType.EXTERNAL' "$app_dir/application.fam"
require_text 'entry_point="pepper_key_app"' "$app_dir/application.fam"
require_text 'fap_file_assets="files"' "$app_dir/application.fam"
require_text 'fap_category="USB"' "$app_dir/application.fam"
require_text 'fap_description="Hardened U2F and authenticator suite alpha"' "$app_dir/application.fam"
require_text 'fap_version="0.2"' "$app_dir/application.fam"
require_text 'targets=["f7"]' "$app_dir/application.fam"
require_text 'APP_ASSETS_PATH("assets/cert.der")' "$app_dir/u2f_data.c"
require_text 'APP_DATA_PATH("key.u2f")' "$app_dir/u2f_data.c"
require_text 'APP_DATA_PATH("cnt.u2f")' "$app_dir/u2f_data.c"
require_text 'event->type == InputTypeLong' "$app_dir/views/u2f_view.c"
require_text 'pepper_key_constant_time_equal' "$app_dir/u2f.c"
require_text 'u2f_begin_request' "$app_dir/u2f.c"
require_text 'u2f_cancel_pending_request' "$app_dir/u2f.c"
require_text 'len < sizeof(U2fRegisterReq)' "$app_dir/u2f.c"
require_text 'len < sizeof(U2fAuthReq)' "$app_dir/u2f.c"
require_text 'WorkerEvtUnlock' "$app_dir/u2f_hid.c"
require_text 'U2fDataLoadMissing' "$app_dir/u2f_data.h"
require_text 'flipper_format_file_open_new' "$app_dir/u2f_data.c"
require_text 'U2F_STOCK_CERT_LEGACY_ENVELOPE_SIZE' "$app_dir/u2f_data.c"
require_text 'u2f_scene_main_stop_session(app);' "$app_dir/scenes/u2f_scene_main.c"
require_text 'PepperHashSha1' "$app_dir/pepper_crypto.h"
require_text 'PepperHashSha256' "$app_dir/pepper_crypto.h"
require_text 'PepperHashSha512' "$app_dir/pepper_crypto.h"
require_text 'pepper_base32_decode' "$app_dir/pepper_base32.c"
require_text 'pepper_hotp_generate' "$app_dir/pepper_otp.c"
require_text 'pepper_totp_generate' "$app_dir/pepper_otp.c"
require_text 'pepper_vault_derive_namespace_key' "$app_dir/pepper_vault.c"
require_text 'PepperVaultFido2' "$app_dir/pepper_vault.h"
require_text 'PepperVaultOath' "$app_dir/pepper_vault.h"
require_text 'PepperVaultChallenge' "$app_dir/pepper_vault.h"
require_text 'pepper_challenge_response' "$app_dir/pepper_challenge.c"
require_text '.usb_cbor_transport = false' "$app_dir/pepper_ctap2.c"
require_text 'const uint8_t pepper_ctap2_aaguid' "$app_dir/pepper_ctap2.c"
forbid_text 'EXT_PATH("u2f/' "$app_dir/u2f_data.c"
forbid_text 'memcmp(req->key_handle.hash' "$app_dir/u2f.c"
forbid_text 'Key loading error, generating new' "$app_dir/u2f.c"
forbid_text 'Counter loading error, resetting counter' "$app_dir/u2f.c"
forbid_text '"Data", key, 48' "$app_dir/u2f_data.c"
forbid_text '"Data", key_encrypted, 48' "$app_dir/u2f_data.c"
forbid_text '"Data", cnt_encr, 48' "$app_dir/u2f_data.c"

sha256_file() {
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$1" | awk '{print $1}'
    elif command -v shasum >/dev/null 2>&1; then
        shasum -a 256 "$1" | awk '{print $1}'
    else
        printf '%s\n' 'no SHA-256 tool found' >&2
        exit 1
    fi
}

check_sha256() {
    expected=$1
    file=$2
    actual=$(sha256_file "$file")
    if [ "$actual" != "$expected" ]; then
        printf 'SHA-256 mismatch: %s\n' "$file" >&2
        exit 1
    fi
}

check_sha256 \
    '6cfb7adf2808b887c7e04afb96242e904f28f940993a4fb1ad07150f327d9ffe' \
    "$app_dir/files/assets/cert.der"
check_sha256 \
    '9eadb4088b517fc1f24514bbee2105414747509155e622dcf979fcb37bde73ae' \
    "$app_dir/files/assets/cert_key.u2f"

stock_data_values=$(awk '/^Data:/ {print NF - 1; exit}' "$app_dir/files/assets/cert_key.u2f")
if [ "$stock_data_values" != '48' ]; then
    printf 'unexpected stock certificate-key envelope size: %s\n' "$stock_data_values" >&2
    exit 1
fi

printf '%s\n' 'PepperKey source checks passed.'
