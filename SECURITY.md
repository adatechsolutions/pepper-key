# PepperKey security and threat model

## Security objective

PepperKey aims to require an owner-present physical action before a bounded authenticator operation, keep protocol identities isolated, reject malformed or stale state, and leave enough evidence to distinguish implemented, device-tested, compatible, and released behavior.

It does not try to convert Flipper Zero into certified or tamper-resistant authenticator hardware. A malicious firmware image, physical attacker with sufficient capability, compromised host, or credential-export mistake can defeat application-level controls.

## Assets

- Legacy U2F credential-derivation root and authentication counter.
- Future CTAP2 private keys, resident credential metadata, PIN/UV state, and extension secrets.
- OATH seeds and HOTP counters.
- Challenge-response keys.
- Future PIV/OpenPGP private keys, certificates, and retry state.
- Recovery packages, recovery metadata, and account recovery codes.
- Approval intent, relying-party binding, protocol state, build provenance, and audit evidence.

## Trust boundaries

1. Flipper application code and RAM.
2. Device enclave-backed encryption service.
3. Removable microSD app data.
4. USB host and browser/client software.
5. Optional future Bluetooth or NFC transport.
6. Relying party and account-recovery system.
7. Build, signing, release, and update infrastructure.

The host, removable storage, radio peer, relying party, and update source are not assumed trustworthy merely because they are connected.

## Required domain isolation

PepperKey is one product shell with separate cryptographic domains. `legacy-fido`, `fido2`, `oath`, `challenge`, `smart-card`, and `recovery` must use distinct labeled roots, records, quotas, parsers, approval contexts, and reset operations. One module may not read another module's secrets. No future migration may silently repurpose the device-tested v0.1 U2F root.

`app/pepper_vault.c` establishes the initial labeled derivation boundary. It is not a persistent encrypted vault by itself. Each module still requires authenticated record storage, atomic updates, corruption tests, rollback analysis, and a destructive-reset ceremony.

## Defensive controls in device-tested U2F

| Risk | Control |
|---|---|
| Accidental launch/unattended connection | Long-hold arming and two-minute maximum session |
| Accidental or cross-request approval | Long-hold approval bound to request type and full 32-byte application hash |
| Unknown-handle prompt spam | Handle validation before prompting |
| Legacy no-presence signing | Presence required for every signature |
| Comparison timing leakage | Constant-time key-handle verification |
| Malformed USB messages | Command, request, key-handle, and frame bounds checks |
| Counter reuse after failed storage | Next counter persisted before signed response |
| Silent identity replacement | Provisioning only when both records are conclusively absent |
| Partial provisioning | Create-only writes and attempt-scoped rollback |
| Stack-data disclosure in records | Exact 32-byte ciphertext serialization |
| Error leaving USB/keys active | Shared teardown stops HID, restores USB, and zeroizes/frees the session |
| Worker/UI queue deadlock | Coalesced event flags with terminal DataError priority |
| Stock-app policy bypass | Separate app ID, assets, data root, and counter |

## New portable-core controls

- Strict Base32 decoding rejects misplaced padding, invalid characters, impossible symbol counts, and non-zero residual bits.
- HOTP/TOTP accept only 6–8 digits, supported hash algorithms, non-empty secrets, valid time epochs, and non-zero periods.
- HMAC buffers, moving factors, and failed outputs are zeroized.
- Challenge input is bounded and lives in a separately derived namespace.
- Vault headers are versioned, namespaced, bounded, and reject unknown flags or generation zero.
- CTAP2 has a non-zero PepperKey model AAGUID but the USB CBOR capability remains false until the entire required path is accepted.

## CTAP2/passkey release conditions

The device must not advertise CTAP2 merely because `authenticatorGetInfo` or a CBOR parser exists. The release gate requires, as one reviewed unit:

- CTAPHID channel correctness, cancellation, timeout, lock, keepalive, sequence, and size handling.
- Canonical bounded CBOR parsing and encoding.
- `authenticatorMakeCredential`, `authenticatorGetAssertion`, `authenticatorGetNextAssertion`, and `authenticatorGetInfo`.
- Discoverable-credential quotas and deterministic deletion/credential management.
- PIN/UV protocol, bounded retry handling, `credProtect`, and `hmac-secret`/PRF.
- Atomic state changes, corruption handling, power-loss behavior, reset ceremony, and reboot persistence.
- Browser, OpenSSH, conformance, negative, and cross-origin tests.

Because the hardware has no application-controlled monotonic counter, PIN retries and rollback-sensitive state require an explicit residual-risk decision before release.

## OATH and challenge-response release conditions

The RFC 4226/6238 cores are not enough to store live seeds. Release requires an encrypted authenticated vault, safe on-device provisioning, redacted UI, RTC drift handling, atomic HOTP increments, zeroization, per-record quotas, backup policy, and controlled deletion. Seed export and keyboard typing remain disabled by default.

Challenge-response additionally requires a documented authenticated USB protocol, replay/domain separation, bounded request sizes, request-bound approval, and a host client that never logs challenges or responses.

## Smart-card and wireless boundaries

`piv` and `openpgp-card` require a USB CCID/composite interface, APDU parser, PIN/retry policy, certificate/key provisioning, isolated key roles, and interoperability suites. The external FAP API currently used for U2F is not treated as proof that CCID is safely available.

`ble-transport` and `nfc-transport` require independent pairing/relay/privacy/downgrade/lifecycle threat models. No proprietary “U2F over BLE/NFC” mode will be shipped.

## Residual risks and non-goals


## Secrets and diagnostics

Never commit or attach credential records, OTP seeds, private keys, PINs, recovery codes, device identifiers, authentication captures, app-data directories, application hashes, key handles, challenges, responses, or decrypted material. Tests use published vectors and synthetic keys only.

## Reporting a vulnerability

Use GitHub private vulnerability reporting in `adatechsolutions/pepper-key`. Do not open a public issue. Include the affected commit/version, a redacted reproduction, impact, and proposed mitigation without live secrets or identifiers.

## Review requirement

Treat every non-`accepted` capability in `capabilities.json` as unavailable. A capability changes to `accepted` only with a dated source review, exact artifact receipt, device/fault acceptance, compatibility matrix, recovery result, and updated claims guide. Do not make PepperKey the sole authenticator for an important account.
