# Contributing to PepperKey

PepperKey accepts focused contributions that preserve its USB-only, fail-closed security model.

## Before opening a change

- Read `SECURITY.md`, `UPSTREAM.md`, and `ACCEPTANCE.md`.
- Do not include device identifiers, account details, app-data files, recovery codes, captured authentication traffic, or decrypted key material.
- Keep Bluetooth/BLE, NFC, CTAP2, passkeys, and certification claims out of the v0.1 release line.
- Open security issues through GitHub private vulnerability reporting, not a public issue.

## Local checks

Use uFBT 0.2.6 with the official F7 SDK 1.4.3:

```sh
sh tests/check-source.sh
sh tests/check-security-fixes.sh
(cd app && ufbt lint)
```

AdaTech release builds must use the guarded command in `BUILD.md`. A build is not device or browser acceptance.

## Pull requests

- Explain the threat or product behavior being changed.
- Include focused test evidence and identify any device/browser paths not tested.
- Preserve the stock asset checksums unless the change is an explicitly reviewed upstream migration.
- Keep the patch reviewable and avoid unrelated formatting churn.
- By contributing, you confirm you have the right to submit the work under GPL version 3.
