# Compatibility evidence

This matrix records observed results for an exact capability, artifact, device, firmware, host, client, and relying party. Blank or untested cells are not failures, but they are not compatibility claims.

## Current device-tested result

| Capability | Device/firmware | Host/client | Relying party | Result | Scope |
|---|---|---|---|---|---|
| `u2f` | Official Flipper Zero F7, stock 1.4.3/API 87.1 | macOS Safari | WebAuthn.io disposable profile | Registration PASS | Exact installed v0.1 FAP only |
| `u2f` | Same | Same | Same | Authentication PASS | Same session after re-arm |
| `u2f` | Same | Same | Same | Reboot persistence PASS | Post-power-cycle authentication |
| `u2f` | Same | macOS USB inspection | N/A | One-shot lock and management USB restoration PASS | Exact installed v0.1 FAP only |

Installed/read-back candidate:

- Size: 69,096 bytes.
- SHA-256: `e22b2cd543ad7bc02d4dd55b3b31770a8900a08065c585e1f7a7fb9ea62b445a`.
- USB identity while armed: Flipper Devices `U2F Token`, VID `0x0483`, PID `0x5741`.
- WebAuthn AAGUID display: all zeros, expected for CTAP1/U2F.

## Open U2F compatibility/fault cells

- Two-minute idle timeout.
- Unknown-handle/check-only no-prompt behavior through a harness.
- Second relying party and cross-request fingerprint isolation.
- Controlled record corruption and incomplete-state behavior.
- Notification flood plus terminal-error USB teardown.
- Same-device recovery restore.
- Chrome, Firefox, Windows, Linux, Android, and additional relying parties.

## Alpha capability matrix

| Capability | Portable core | FAP build | Device UI/storage | Host protocol | Compatibility |
|---|---|---|---|---|---|
| `oath-hotp` | PASS | CI FAP PASS; local guarded build held | Not implemented | N/A | Not tested |
| `oath-totp` | PASS | CI FAP PASS; local guarded build held | Not implemented | N/A | Not tested |
| `challenge-response` | PASS | CI FAP PASS; local guarded build held | Not implemented | Not implemented | Not tested |
| `ctap2` | Fail-closed gate PASS | CI FAP PASS; local guarded build held | Not implemented | Disabled | Not tested |
| `passkeys` | Architecture only | N/A | Not implemented | Disabled | Not tested |
| `ssh-fido` | Architecture only | N/A | Not implemented | Disabled | Not tested |
| `piv` | Research only | N/A | Not implemented | Disabled | Not tested |
| `openpgp-card` | Research only | N/A | Not implemented | Disabled | Not tested |
| `ble-transport` | Research only | N/A | Not implemented | Disabled | Not tested |
| `nfc-transport` | Research only | N/A | Not implemented | Disabled | Not tested |
| `recovery` | Policy only | N/A | Same-device restore untested | Disabled | Not tested |

Alpha CI receipt:

- Source commit: `7dde1683a46d9ca3a9b648554504ed9ce77fc305`.
- Run: `33664964327`.
- FAP SHA-256: `df92ccbcdb724163c2550d1ba11c1b957eb39db4b0c0698674e075d4184bca18`.
- Device status: not installed, not device-tested.

## Adding a result

Record:

1. PepperKey capability ID/version/commit/artifact hash.
2. Flipper model, firmware/API, and relevant settings.
3. Host OS/hardware, browser/client and versions.
4. Relying party or synthetic harness and test account class.
5. Operation, expected result, actual result, timestamp, and redacted evidence.
6. Whether another authenticator and recovery path remained available.

Never include credential IDs, user handles, application/RP hashes, key handles, OTP seeds/codes, challenges/responses, PINs, account names, device serials, app-data files, or recovery codes.
