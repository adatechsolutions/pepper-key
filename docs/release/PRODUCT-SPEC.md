# PepperKey 1.0 product specification

## Ownership and release line

- Product owner and publisher: Ada Tech Solutions Inc.
- Canonical source: `https://github.com/adatechsolutions/pepper-key`
- License: GNU GPL version 3.
- Hardware: Flipper Zero target F7.
- Device-tested line: v0.1 USB U2F/CTAP1 on official firmware/SDK 1.4.3 / API 87.1, with remaining fault/recovery/release gates.
- Development line: 0.2 alpha modular authenticator foundation.
- Intended 1.0 surface: USB FIDO2/U2F, on-device OATH, challenge-response, and explicitly separated research tracks for smart-card and wireless transports.

## Intended use

PepperKey is a secondary, owner-present experimental authenticator for enthusiasts, controlled labs, training, development, and carefully tested non-critical use. One launcher may expose several capabilities, but the internal identities, stores, approvals, resets, and release evidence remain independent.

It is not a sole authenticator, certified authenticator, biometric verifier, cloud passkey provider, enterprise identity platform, or hardware-equivalent replacement for a certified security key.

## Product principles

1. Locked by default.
2. Local physical approval bound to one parsed request.
3. One product shell, multiple isolated cryptographic domains.
4. No capability advertisement before the complete implementation is accepted.
5. Bounded messages, credentials, retries, time, and memory.
6. Versioned records, atomic transitions, fail-closed corruption handling, and explicit destructive reset.
7. Exact source/SDK/artifact/device/compatibility evidence for every claim.
8. The device-tested v0.1 U2F identity is never silently migrated or repurposed.

## Component requirements

### Legacy FIDO

Preserve the device-tested v0.1 U2F behavior and record paths. Its state machine remains locked, arming, armed, pending, approved, locking, or terminal error. No other module may read its device root or counter.

### FIDO2 and passkeys

Implement CTAP2.2 USB HID and canonical CBOR with ES256, discoverable and non-discoverable credentials, PIN/UV, credential management, `credProtect`, `hmac-secret`/PRF, reset/configuration, quotas, and power-loss-safe state. PepperKey CTAP2 model AAGUID: `34e4a441-9a60-4361-830a-cfa20bff3147`.

No CTAP2 capability bit or `FIDO_2_*` version may be advertised before makeCredential, getAssertion, GetInfo, PIN/UV requirements, persistence, and acceptance pass together.

### OATH

Support RFC 4226 HOTP and RFC 6238 TOTP with SHA-1, SHA-256, SHA-512, 6–8 digits, configurable period, bounded profile count, encrypted authenticated records, RTC drift indication, explicit reveal/typing policy, atomic counter updates, and per-entry deletion. Seed export is off by default.

### Challenge-response

Support bounded HMAC-SHA-256 challenges with per-slot isolated keys, domain-separated messages, replay guidance, request fingerprinting, explicit approval, and a redacting host client. Do not reuse FIDO or OATH keys.

### Smart card

PIV and OpenPGP require a standards-compatible CCID/composite USB layer, isolated key roles, APDU bounds, PIN/retry policy, certificate/key provisioning, reset/recovery policy, and interoperability evidence. They are not part of the external-FAP alpha release.

### Recovery

Same-device backup remains the only supported accepted path. Any portable encrypted export needs a versioned authenticated format, explicit ceremony, strong user-supplied protection, clone/rollback analysis, import quotas, and a visible record of portability. Account recovery codes remain outside PepperKey by default.

### Wireless

Bluetooth and NFC are separate products/security transports. They require standards compatibility and independent pairing, privacy, relay, downgrade, field-power, disconnect, and teardown acceptance.

## Release requirements

Each capability requires exact reviewed source, immutable commit, pinned build inputs, artifact digest, independent security review, physical-device and fault acceptance, compatibility evidence, recovery result, genuine screenshots, updated privacy/support/claims materials, and—where applicable—formal certification. Source merge or a green unit test alone is not release evidence.
