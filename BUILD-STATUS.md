# PepperKey build status

Status: `V0.1_PARTIAL_DEVICE_ACCEPTANCE / V0.2_ALPHA_IMPLEMENTATION`

Date: 2026-09-02

## Device-tested v0.1 evidence

- Official Flipper Zero firmware 1.4.3 / target F7 / API 87.1.
- Exact installed and device-read-back FAP: 69,096 bytes, SHA-256 `e22b2cd543ad7bc02d4dd55b3b31770a8900a08065c585e1f7a7fb9ea62b445a`.
- Stock `/ext/apps/USB/u2f.fap` and `/ext/Manifest` digests remained unchanged.
- Before-state backup at `outputs/device-acceptance/20260902T165400Z-before` contains a qFlipper internal backup plus all 81 `/ext/apps_data` files; 561,269 bytes passed file-by-file MD5 verification.
- PepperKey created only `/ext/apps_data/pepper_key/key.u2f` (203 bytes) and `cnt.u2f` (200 bytes).
- Locked launch, short-press rejection, long-hold arm, U2F HID enumeration, no-request hold, disposable WebAuthn registration, authentication, power-cycle persistence, post-reboot authentication, one-operation lock, management-USB restoration, unchanged key record, and advancing counter record passed.
- WebAuthn displayed all-zero AAGUID, which is expected for CTAP1/U2F; the inherited stock certificate contains no AAGUID extension.

## Remaining v0.1 acceptance

- Two-minute idle timeout.
- Unknown-handle and check-only no-prompt harness cases.
- Second relying-party/application-hash isolation.
- Controlled corruption and fail-closed persistence cases.
- Notification flood and terminal-error teardown.
- Same-device recovery restore.
- Genuine screenshot, immutable release receipt, Catalog validator, and Catalog moderation.

These open gates prohibit calling v0.1 a complete production release.

## 0.2 alpha implementation

Completed in source:

- Strict Base32 decoding foundation.
- RFC 4226 HOTP and RFC 6238 TOTP core with SHA-1/SHA-256/SHA-512 and 6–8 digits.
- HMAC-SHA-256 challenge-response primitive.
- Labeled FIDO2/OATH/challenge/smart-card/recovery namespace derivation.
- Versioned and bounded vault-header validation.
- Stable PepperKey CTAP2 AAGUID plus an explicit false capability gate.
- Native published-vector, isolation, parser, and fail-closed CTAP2 advertisement tests.
- Machine-readable capability registry and claim policy.
- GitHub CI passed commit `7dde1683a46d9ca3a9b648554504ed9ce77fc305` in run `33664964327` using pinned uFBT 0.2.6 and official F7 SDK 1.4.3.
- CI built `pepper_key.fap` with SHA-256 `df92ccbcdb724163c2550d1ba11c1b957eb39db4b0c0698674e075d4184bca18`.
- The uploaded CI artifact envelope digest is `32f009a57965c1a1a0b020be98d1f872dfce4d59210582d6b05b58cb66d3b69e`.
- The CI FAP is not installed or device-accepted and must not replace the v0.1 artifact currently on Pepper.
- The guarded Pro13 build passed source head `37b0569961c8c95242c67b7d217a481bb96f2d95` with a 69,096-byte FAP, SHA-256 `2efecbd26dc87623ac2b06aca07f6c56d289759696cf5ebe607af172b65bfaa7`.
- The corresponding CI FAP is also 69,096 bytes. It differs from the Pro13 FAP at exactly four bytes: the host-specific CRC in `.gnu_debuglink` (section offset `0x8ab8`, size 24), matching the documented cross-host build behavior.

Not yet available on the device:

- CTAP2 USB CBOR, makeCredential/getAssertion, resident passkeys, PIN/UV, credential management, extensions, or SSH FIDO acceptance.
- Encrypted OATH profile storage, provisioning UI, code display/typing, RTC integration, or atomic HOTP counter persistence.
- Challenge-response USB transport and physical approval UI.
- PIV/OpenPGP CCID firmware, Bluetooth authentication, NFC authentication, or portable recovery.

## Alpha build evidence

GitHub CI and the separate guarded Pro13 build both completed with pinned uFBT 0.2.6, F7 SDK 1.4.3, and API 87.1. Earlier attempts were correctly held while Pro13 exceeded the fleet load threshold. Neither alpha artifact has been installed or device-accepted.

## Rebuild commands

Portable suite checks:

```sh
./tests/check-core.sh
```

Guarded FAP build:

```sh
./scripts/build-release.sh
```

The release script builds only. It never launches, installs, migrates, enrolls, or resets credentials.

Any 0.2 source change invalidates the v0.1 FAP digest as evidence for the new source. Preserve the device-tested v0.1 artifact and rebuild 0.2 from an immutable reviewed commit before device testing.
