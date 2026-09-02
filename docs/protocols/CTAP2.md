# CTAP2 and passkeys

## Standard target

The implementation target is FIDO Client to Authenticator Protocol 2.2, with WebAuthn Level 3 behavior validated through browsers and relying parties. CTAP1/U2F fallback remains available through the device-tested legacy domain.

PepperKey model AAGUID: `34e4a441-9a60-4361-830a-cfa20bff3147`.

The AAGUID identifies the PepperKey authenticator model; it is not a secret or per-device identifier. The all-zero AAGUID observed for the device-tested U2F flow remains correct because CTAP1/U2F does not transmit a CTAP2 AAGUID.

## Mandatory implementation slice

1. CTAPHID INIT, PING, MSG, CBOR, CANCEL, ERROR, KEEPALIVE, WINK, LOCK, channel sequencing, and timeouts.
2. Canonical, bounded CBOR reader/writer with duplicate-key and trailing-data rejection.
3. `authenticatorGetInfo`.
4. `authenticatorMakeCredential` using ES256.
5. `authenticatorGetAssertion` and `authenticatorGetNextAssertion`.
6. Non-discoverable and discoverable credentials.
7. `clientPIN` PIN/UV protocol and retry policy.
8. Credential management for resident credentials.
9. `credProtect` and `hmac-secret`/WebAuthn PRF.
10. Authenticator configuration/reset with a destructive local ceremony.
11. Atomic persistence, quotas, corruption handling, reboot behavior, and credential deletion.
12. Cross-origin, allow/exclude-list, user-presence, user-verification, cancellation, timeout, and malformed-input tests.

`largeBlob` and `credBlob` follow only after the mandatory path is stable and measured.

## Current source state

`app/pepper_ctap2.c` assigns the AAGUID and exposes a fail-closed readiness structure. Every flag is false. This is intentional: a partial GetInfo implementation must not cause browsers to route passkey requests into an authenticator that cannot complete them.

## Credential model

Each credential includes a bounded credential ID, RP ID hash, user handle, private key or wrapped-key material, signature counter policy, discoverability flag, protection policy, creation generation, and authenticated metadata. RP display strings are untrusted and length-bounded.

FIDO2 state lives under its own vault domain and cannot use the v0.1 U2F root, stock U2F attestation key, OATH secrets, challenge keys, or recovery encryption key.

## Attestation

Development builds use none or self attestation as permitted by the selected conformance profile. The inherited stock Flipper U2F certificate/private key may not be used to claim PepperKey CTAP2 model identity. Batch attestation requires a controlled provisioning system, certificate policy, protected signing keys, privacy review, rotation/revocation plan, and appropriate vendor/certification agreements.

## PIN and user verification

Flipper Zero has no biometric verifier. PIN is the available CTAP2 user-verification mechanism. The UI must resist shoulder surfing as reasonably possible, clear PIN buffers, rate-limit attempts, and never log the PIN. Because application state lacks a hardware monotonic counter, rollback-resistant retry semantics cannot be guaranteed without additional trusted hardware; this is a release-blocking risk decision, not a hidden limitation.

## SSH

OpenSSH FIDO-backed key support is a compatibility profile of the accepted CTAP2 implementation, not a separate key-export feature. Acceptance requires disposable `ed25519-sk` or `ecdsa-sk` enrollment as supported by the implementation, user-presence signing, reboot persistence, resident/non-resident behavior, failure/recovery tests, and exact OpenSSH version recording.

## Release gate

Do not change the HID INIT capability byte to advertise CBOR and do not return `FIDO_2_*` from GetInfo until all mandatory commands and security state are integrated and pass:

- native parser/vector tests,
- sanitizer/fuzz corpus on a host build,
- pinned uFBT lint/build,
- exact-artifact install/read-back,
- FIDO conformance testing where available,
- macOS Safari/Chrome/Firefox and at least one non-macOS host,
- WebAuthn registration/login/reboot/deletion/reset,
- OpenSSH profile,
- power-loss/corruption/flood/cancellation tests,
- independent security review.

## Primary specifications

- FIDO CTAP 2.2 Proposed Standard, 2025-07-14: <https://fidoalliance.org/specs/fido-v2.2-ps-20250714/fido-client-to-authenticator-protocol-v2.2-ps-20250714.html>
- W3C Web Authentication Level 3: <https://www.w3.org/TR/webauthn-3/>
