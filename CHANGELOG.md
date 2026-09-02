# Changelog

## 0.2.0-alpha.1 — modular authenticator foundation

- Defined PepperKey 1.0 as one product shell with separate FIDO2, OATH, challenge, smart-card, and recovery security domains.
- Added strict Base32 decoding suitable for bounded OTP provisioning inputs.
- Added RFC 4226 HOTP and RFC 6238 TOTP cores supporting SHA-1, SHA-256, SHA-512, and 6–8 digits.
- Added an isolated HMAC-SHA-256 challenge-response primitive.
- Added labeled per-domain key derivation and a versioned, bounded vault-record header.
- Assigned PepperKey CTAP2 AAGUID `34e4a441-9a60-4361-830a-cfa20bff3147`.
- Added a fail-closed CTAP2 readiness API; USB CBOR remains disabled until the complete authenticator path is accepted.
- Added native RFC vectors, namespace isolation, parser rejection, and capability-policy tests.
- Added machine-readable implementation states in `capabilities.json`.
- Expanded the product specification, threat model, protocol plans, release gates, recovery model, and claims policy.

This alpha does not advertise or provide CTAP2/passkeys, OTP vault UI, challenge-response transport, PIV, OpenPGP, BLE/NFC authentication, or portable credential recovery on-device.

## 0.1.0 — release candidate

- Derived an external FAP from the official Flipper Zero U2F application at firmware 1.4.3.
- Added locked-by-default long-hold arming and per-request approval.
- Added full application-hash/request-type approval binding and pre-prompt handle validation.
- Added one-success lock, request/session timeouts, parser checks, constant-time comparison, and secret zeroization.
- Added fail-closed paired identity provisioning and create-only first-run records.
- Limited new encrypted records to exact 32-byte ciphertext fields.
- Added terminal-error USB teardown through a non-blocking worker/UI event bridge.
- Added pinned build/CI, source checks, security review summary, acceptance gates, support/legal materials, and Catalog preparation.

The version remains a release candidate until physical device, disposable-account, fault, recovery, screenshot, immutable commit, and Catalog moderation gates pass.
