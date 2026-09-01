#!/bin/sh
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
app_dir=$(CDPATH= cd -- "$script_dir/../app" && pwd)
project_dir=$(CDPATH= cd -- "$script_dir/.." && pwd)

resource_check=${CODEX_RESOURCE_CHECK_BIN:-$(command -v codex-resource-check || true)}
heavy_run=${CODEX_HEAVY_RUN_BIN:-$(command -v codex-heavy-run || true)}
ufbt=${UFBT_BIN:-$(command -v ufbt || true)}

if [ -z "$resource_check" ] || [ -z "$heavy_run" ] || [ -z "$ufbt" ]; then
    printf '%s\n' \
        'PepperKey release build held: configure codex-resource-check, codex-heavy-run, and ufbt.' >&2
    exit 77
fi

if ! resource_output=$($resource_check 2>&1); then
    printf '%s\n' "$resource_output" >&2
    printf '%s\n' 'PepperKey build held: AdaTech resource gate did not pass.' >&2
    exit 75
fi

printf '%s\n' "$resource_output"

ufbt_status=$($ufbt status 2>&1)
printf '%s\n' "$ufbt_status"
if ! printf '%s\n' "$ufbt_status" | grep -Eq 'uFBT version[[:space:]]+0\.2\.6$'; then
    printf '%s\n' 'PepperKey build held: expected uFBT 0.2.6.' >&2
    exit 76
fi
if ! printf '%s\n' "$ufbt_status" | grep -Eq 'Target[[:space:]]+f7$'; then
    printf '%s\n' 'PepperKey build held: expected target f7.' >&2
    exit 76
fi
if ! printf '%s\n' "$ufbt_status" | grep -Eq 'Version[[:space:]]+1\.4\.3$'; then
    printf '%s\n' 'PepperKey build held: expected SDK 1.4.3.' >&2
    exit 76
fi

sh "$project_dir/tests/check-source.sh"
sh "$project_dir/tests/check-security-fixes.sh"
(cd "$app_dir" && "$ufbt" lint)

cd "$app_dir"
exec "$heavy_run" -- "$ufbt" faps
