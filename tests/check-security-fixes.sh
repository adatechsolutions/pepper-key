#!/bin/sh
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
project_dir=$(CDPATH= cd -- "$script_dir/.." && pwd)
data_file=$project_dir/app/u2f_data.c
core_file=$project_dir/app/u2f.c
scene_file=$project_dir/app/scenes/u2f_scene_main.c
app_file=$project_dir/app/u2f_app.c
base32_file=$project_dir/app/pepper_base32.c
otp_file=$project_dir/app/pepper_otp.c
vault_file=$project_dir/app/pepper_vault.c
challenge_file=$project_dir/app/pepper_challenge.c
ctap2_file=$project_dir/app/pepper_ctap2.c

require_text() {
    needle=$1
    file=$2
    if ! grep -Fq -- "$needle" "$file"; then
        printf 'missing security invariant: %s in %s\n' "$needle" "$file" >&2
        exit 1
    fi
}

forbid_text() {
    needle=$1
    file=$2
    if grep -Fq -- "$needle" "$file"; then
        printf 'forbidden security pattern: %s in %s\n' "$needle" "$file" >&2
        exit 1
    fi
}

require_text 'U2F_CIPHERTEXT_SIZE' "$data_file"
require_text '_Static_assert(sizeof(U2fCounterData) == U2F_CIPHERTEXT_SIZE' "$data_file"
require_text 'stored_data_size != data_size' "$data_file"
require_text 'state && cert_type == U2F_CERT_USER_UNENCRYPTED' "$data_file"
require_text 'flipper_format_file_open_new(flipper_format, U2F_KEY_FILE)' "$data_file"
require_text 'flipper_format_file_open_new(flipper_format, U2F_CNT_FILE)' "$data_file"
require_text 'if(file_created && !state)' "$data_file"
require_text 'key_result == U2fDataLoadMissing && counter_result == U2fDataLoadMissing' "$core_file"
require_text 'u2f_data_key_remove_created()' "$core_file"
require_text 'u2f_scene_main_stop_session(app);' "$scene_file"
require_text 'furi_event_flag_set(app->notification_flags' "$scene_file"
require_text 'furi_event_loop_subscribe_event_flag' "$app_file"
require_text 'A terminal error supersedes all queued/coalesced protocol notifications.' "$app_file"

require_text 'PepperBase32NonCanonicalBits' "$base32_file"
require_text 'pepper_secure_zero(output, written)' "$base32_file"
require_text 'digits >= 6 && digits <= 8' "$otp_file"
require_text 'pepper_secure_zero(digest, sizeof(digest))' "$otp_file"
require_text 'header->namespace_id != (uint8_t)expected_namespace' "$vault_file"
require_text 'PepperKey/v1/vault/' "$vault_file"
require_text 'challenge_size > PEPPER_CHALLENGE_MAX_SIZE' "$challenge_file"
require_text '.usb_cbor_transport = false' "$ctap2_file"
require_text 'if(!readiness.usb_cbor_transport) return false;' "$ctap2_file"

forbid_text '.usb_cbor_transport = true' "$ctap2_file"
forbid_text 'FIDO_2_0' "$ctap2_file"
forbid_text 'FIDO_2_1' "$ctap2_file"

forbid_text 'uint8_t key_encrypted[48]' "$data_file"
forbid_text 'uint8_t cnt_encr[48]' "$data_file"
forbid_text 'flipper_format_write_hex(flipper_format, "Data", key, 48)' "$data_file"
forbid_text 'flipper_format_write_hex(flipper_format, "Data", key_encrypted, 48)' "$data_file"
forbid_text 'flipper_format_write_hex(flipper_format, "Data", cnt_encr, 48)' "$data_file"
forbid_text 'Key loading error, generating new' "$core_file"
forbid_text 'Counter loading error, resetting counter' "$core_file"
forbid_text 'view_dispatcher_send_custom_event(app->view_dispatcher, U2fCustomEventDataError)' "$scene_file"

printf '%s\n' 'PepperKey security-fix source checks passed.'
