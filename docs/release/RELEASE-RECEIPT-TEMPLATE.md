# PepperKey release receipt

Do not place live credentials, account/device identifiers, app data, OTP material, PINs, authentication captures, challenge/response values, or recovery packages in this receipt.

## Release identity

- Capability IDs:
- Version:
- Source commit (40 characters):
- Branch/tag:
- Reviewer(s):
- Review date/time zone:
- Capability status before/after:

## Standards and dependencies

- Protocol/specification versions:
- Firmware/SDK version and API:
- Firmware source commit:
- uFBT/toolchain version:
- Dependency digests:
- Upstream diff reviewed:

## Source verification

- `./tests/check-core.sh`:
- `tests/check-source.sh`:
- `tests/check-security-fixes.sh`:
- uFBT lint:
- Additional unit/vector/fuzz/sanitizer/conformance suites:
- Independent security review result:
- `git diff --check`:
- Clean worktree:

## Artifact

- Build host role:
- Resource-gate receipt:
- Heavy-job lock receipt:
- Build command:
- Artifact path/name:
- Size:
- SHA-256:
- CI run and CI artifact digest:
- Expected host-specific differences:

## Device

- Device model/target (identifier redacted):
- Firmware/API:
- Battery/storage/temperature/USB ownership:
- Before-state backup path and manifest:
- Installed path:
- Device read-back digest:
- Stock files checked/unchanged:
- App data paths created/changed:

## Functional and negative acceptance

- Locked/arm/approval/timeout:
- Registration/provisioning:
- Authentication/generation/signing:
- Reboot persistence:
- Cancellation/disconnect/USB restoration:
- Malformed/oversized/duplicate/trailing input:
- Unknown credential/account/slot:
- Quota and resource exhaustion:
- Corruption/incomplete/wrong-version/wrong-device state:
- Persistence-write failure and power loss:
- Flood/concurrency/teardown:
- Reset/deletion:

## Compatibility

For each row: capability, host OS, client/browser, relying party/harness, operation, result, timestamp, and redacted evidence pointer.

## Recovery and rollback

- Independent authenticator/recovery path verified:
- Same-device recovery:
- Portable recovery, if applicable:
- Clone/rollback behavior:
- Application rollback:
- Credential removal/re-enrollment:

## Privacy, support, and claims

- Privacy review:
- Support scope:
- Claims guide updated:
- Genuine screenshots checked/redacted:
- Catalog/release metadata:
- Certification statement:

## Decision

- Final status: PASS / HOLD / FAIL
- Open items:
- Stop conditions:
- Approver and date:
