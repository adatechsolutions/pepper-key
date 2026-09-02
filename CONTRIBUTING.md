# Contributing to PepperKey

PepperKey accepts focused contributions that preserve fail-closed capability advertisement, protocol isolation, the accepted U2F baseline, and evidence-bound claims.

## Before opening a change

- Read `SECURITY.md`, `UPSTREAM.md`, `ACCEPTANCE.md`, `capabilities.json`, and the relevant protocol/architecture decisions.
- Do not include device identifiers, account details, app-data files, recovery codes, OTP seeds/codes, PINs, challenges/responses, captured authentication traffic, or decrypted key material.
- Keep Bluetooth/BLE, NFC, CTAP2, passkeys, smart-card, recovery, and certification claims out of the v0.1 release line.
- Do not mark a capability `accepted` without its exact artifact/device/compatibility/fault/recovery evidence.
- New persistent state needs a versioned schema, quotas, authenticated encryption, atomic update, rollback analysis, corruption tests, destructive reset, and recovery policy.
- Open security issues through GitHub private vulnerability reporting, not a public issue.

## Local checks

Use uFBT 0.2.6 with the official F7 SDK 1.4.3:

```sh
./tests/check-core.sh
sh tests/check-source.sh
sh tests/check-security-fixes.sh
(cd app && ufbt lint)
```

AdaTech release builds must use the guarded command in `BUILD.md`. A build is not device or browser acceptance.

## Pull requests

- Explain the threat or product behavior being changed.
- Name every affected capability ID and security domain.
- Keep unfinished protocol advertisement disabled.
- Update code, tests, `capabilities.json`, product/security/protocol docs, acceptance gates, and claims together.
- Include focused test evidence and identify any device/browser paths not tested.
- Preserve the stock asset checksums unless the change is an explicitly reviewed upstream migration.
- Keep the patch reviewable and avoid unrelated formatting churn.
- By contributing, you confirm you have the right to submit the work under GPL version 3.
