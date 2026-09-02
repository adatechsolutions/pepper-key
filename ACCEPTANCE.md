# PepperKey acceptance plan

Each section is an independent gate. Do not infer later acceptance from an earlier success.

## 1. Build gate

- Kepler resource check passes and no competing heavy-job owner exists.
- Official firmware and SDK are pinned to tag 1.4.3, commit `8622f1a2b83d8f4918dd5fa3f43de963f6d6f819`, target F7, API 87.1.
- uFBT performs strict FAP import checking.
- Formatting and lint checks pass.
- A clean build produces `pepper_key.fap` and packaged file assets.
- Source and artifact SHA-256 hashes are recorded.

## 2. Pre-install device gate

- Read-only inspection confirms the exact official Flipper Zero target and current firmware/API.
- qFlipper, Flipper Lab, mobile RPC, serial CLI, and firmware tools are closed so there is one mutable owner.
- Battery, temperature, microSD mount, and free space are healthy.
- Current internal storage and microSD app data are backed up with a same-device recovery path.
- A genuine backup authenticator and account recovery codes exist before any enrollment.

## 3. Installation gate

- Install only the reviewed `pepper_key.fap` and its embedded assets.
- Record the installed artifact hash and target path.
- Confirm the existing official resource bundle and stock firmware remain unchanged.
- Confirm launch, back/exit, reboot persistence, and removal/rollback behavior before enrollment.

## 4. Disposable-account functional gate

Use a disposable test account and a browser/service that still accepts U2F authenticators.

1. Launch PepperKey while USB RPC is not active. It must show `PEPPERKEY LOCKED`.
2. A short OK press must not arm it. A long OK hold must arm it.
3. Without a registration or sign-in request, holding OK must not authorize anything.
4. Start registration. PepperKey must show `REGISTER REQUEST` and an eight-character fingerprint.
5. A short press must not approve. A long hold must approve the displayed request.
6. Registration must succeed, show `APPROVED - LOCKING`, and return to locked state after three seconds.
7. Relaunch or re-arm and complete sign-in. It must require a new long hold and then lock again.
8. Leave an armed session unused. It must lock after two minutes.
9. Send a check-only or unknown-handle request with the test harness. It must not show an approval prompt.
10. Confirm a second origin produces a different displayed fingerprint and cannot consume approval for the first origin.
11. Power-cycle Pepper and repeat sign-in. Confirm the authentication counter advances.
12. Repeat on every intended macOS/browser/service combination; record failures rather than generalizing from one success.

## 5. Fail-closed fault gate

Use only the disposable test identity and preserve a before-state backup.

- Confirm clean first run creates a 32-byte encrypted device-key record and a 32-byte encrypted counter record.
- With only `key.u2f` present, arming must fail locked and must not alter either path.
- With only `cnt.u2f` present, arming must fail locked and must not alter either path.
- Corrupt each header, version, IV, Data length, ciphertext, and counter control in a controlled copy. Every case must fail locked with zero automatic replacement/reset writes.
- Fail the second create during clean provisioning. PepperKey may remove only the key created by that same attempt and must permit a later clean retry.
- Fail counter persistence during authentication. No signature may be returned.
- Flood connect/disconnect/register/auth/wink notifications, inject a terminal DataError, and verify bounded teardown: all timers stopped, HID joined, prior USB mode restored, keys freed/zeroized, pointers cleared, and `DATA ERROR - LOCKED` visible.
- Confirm holding OK after the locked error retries safely and performs no write unless both records are conclusively absent.

## 6. Recovery gate

- Back up `/ext/apps_data/pepper_key` without printing or opening its contents.
- Restore the backup only to the same Flipper Zero during a controlled disposable-account test.
- Confirm that copying the app data to another device is not presented as a supported recovery method.
- Prove that the account remains accessible with the independent backup authenticator if the Flipper Zero is unavailable.

## 7. Suite alpha source gate

Before a 0.2 alpha artifact is built:

- `./tests/check-core.sh` passes published HOTP/TOTP vectors, Base32 rejection, domain isolation, challenge-response, vault-header, CTAP2 advertisement, and capability-policy tests.
- v0.1 source/security regression suites still pass.
- uFBT lint passes with no manifest warning.
- `capabilities.json` matches README, security, protocol, support, privacy, and claims documents.
- No generated credentials, OTP seeds/codes, PINs, responses, device data, or recovery material enters source/test logs.
- Independent review confirms the new modules cannot change accepted U2F runtime behavior merely by being compiled.

A core-ready result does not authorize device provisioning or feature advertisement.

## 8. Capability promotion gate

For every capability moving to `accepted`, create a dated receipt containing:

1. Capability ID, security domain, source commit, dependency/specification versions, and reviewer.
2. Exact artifact filename, size, SHA-256, build host, target, SDK, and CI result.
3. Device identity/firmware, before-state backup, installation/read-back, and rollback result.
4. Positive, negative, malformed, timeout, disconnect, cancellation, flood, power-loss, corruption, quota, reset, and recovery results appropriate to the protocol.
5. Exact host OS/client/browser/relying-party matrix with failures retained.
6. Secret/redaction review and genuine screenshots with identifiers removed.
7. Updated support/privacy/claims/catalog/release materials.

### CTAP2/passkeys

CTAPHID CBOR remains unadvertised until GetInfo, makeCredential, getAssertion/getNextAssertion, ES256, resident/non-resident credentials, PIN/UV, credential management, required extensions, atomic storage, cancellation, reset, conformance, browsers, and OpenSSH pass together.

### OATH

No live seed is provisioned until authenticated encrypted storage, bounded parsing, RTC/drift UX, atomic HOTP counter persistence, reveal/typing policy, deletion, corruption, backup, and 24-hour cross-implementation comparison pass.

### Challenge-response

No USB command is exposed until the transcript-bound protocol, host client, replay/domain separation, request-bound approval, slot lifecycle, redaction, and malformed/fault suites pass.

### Smart card and wireless

PIV/OpenPGP require the separate firmware/CCID and interoperability gate. BLE/NFC require independent transport threat models and standards-compatible implementations. They cannot inherit USB acceptance.

### Recovery

Portable export remains disabled until KDF/AEAD format review, clone/rollback UX, synthetic vectors, cross-platform restore, corruption/power-loss tests, and independent recovery remain proven.

## Rollback

Before any account enrollment, rollback is removal of the PepperKey FAP and app-specific assets/data using the backed-up before-state. After enrollment, first remove PepperKey from every relying party while another authenticator is working; only then remove its app data. Never delete app data as the first rollback step.
