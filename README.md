# PepperKey

PepperKey is an open-source authenticator suite for Flipper Zero from [Ada Tech Solutions Inc.](https://adatechsolutions.ca). The device-tested v0.1 capability is a hardened USB U2F/CTAP1 application with remaining fault/recovery/release gates. The 0.2 alpha line adds independently isolated foundations for passkeys, OATH OTP, challenge-response, smart-card research, and recovery without pretending those paths are already device-ready.

Canonical source: `https://github.com/adatechsolutions/pepper-key`

PepperKey is not FIDO Certified, tamper-resistant hardware, a biometric authenticator, a cloud-synced passkey provider, or a drop-in replacement for a purpose-built commercial security key. Every capability is fail-closed behind a machine-readable readiness gate in `capabilities.json`.

## Capability truth table

| Capability ID | Current state | What exists now |
|---|---|---|
| `u2f` | Device-tested on Pepper | USB U2F registration, authentication, reboot persistence, one-shot lock, and artifact read-back passed with a disposable account; fault/recovery/release gates remain |
| `oath-totp` | Core-ready | RFC 6238 SHA-1/SHA-256/SHA-512 implementation and native vectors; encrypted device vault and UI remain gated |
| `oath-hotp` | Core-ready | RFC 4226 implementation and counters 0–9 vectors; atomic device counter persistence remains gated |
| `challenge-response` | Core-ready | HMAC-SHA-256 primitive and isolated namespace derivation; no host transport is advertised |
| `ctap2` | Implementation | Stable AAGUID and fail-closed readiness API; USB CBOR is intentionally disabled |
| `passkeys` | Implementation | Architecture and release gates; no discoverable credentials are advertised |
| `ssh-fido` | Implementation | Depends on accepted CTAP2 registration and assertion paths |
| `recovery` | Implementation | Policy and record boundary; no portable credential export exists |
| `piv` | Research | NIST PIV/CCID architecture only |
| `openpgp-card` | Research | OpenPGP smart-card/CCID architecture only |
| `ble-transport` | Research | Separate standards, pairing, privacy, relay, and teardown track |
| `nfc-transport` | Research | Separate transport and field-power threat model |

`core-ready` does not mean available on the Flipper UI. It means the bounded portable core is implemented and passes its named test vectors. A feature becomes accepted only after source review, a pinned artifact, exact-device testing, compatibility testing, failure injection, and recovery evidence.

## Suite architecture

PepperKey presents one product but keeps independent security domains:

- Legacy FIDO: the proven v0.1 `u2f` identity and counter.
- FIDO2: CTAP2 credentials, PIN/UV state, and extensions.
- OATH: `oath-totp` and `oath-hotp` seeds and counters.
- Challenge: `challenge-response` keys.
- Smart card: `piv` and `openpgp-card` signing/decryption/authentication keys.
- Recovery: encrypted export metadata and restore ceremonies.

The new portable core derives different domain keys using explicit labels and never treats one module's key as another module's root. Wireless transports remain separate because enabling a radio changes the attack surface and lifecycle.

See `docs/architecture/PEPPERKEY-1.0.md`, the protocol specifications under `docs/protocols`, and `SECURITY.md`.

## Device-tested U2F behavior

- Starts locked; a long local OK hold arms a two-minute USB session.
- Requires a long hold for each registration and authentication.
- Binds approval to the complete 32-byte U2F application hash.
- Rejects unknown handles and check-only probes before prompting.
- Locks after one successful operation.
- Persists the next authentication counter before returning a signature.
- Fails closed on incomplete, malformed, unreadable, or undecryptable state.
- Restores the previous USB mode and zeroizes the session after terminal error or lock.

The device-tested U2F records remain:

- `/ext/apps_data/pepper_key/key.u2f`
- `/ext/apps_data/pepper_key/cnt.u2f`

Do not inspect, publish, edit, delete, or casually restore these files. They are bound to the original device's enclave key.

## Development and tests

Run the portable suite checks:

```sh
./tests/check-core.sh
```

Run the v0.1 source/security checks:

```sh
./tests/check-source.sh
./tests/check-security-fixes.sh
```

Run the guarded FAP build only on the authorized AdaTech build host:

```sh
./scripts/build-release.sh
```

The guarded build refuses to run when the AdaTech resource or heavy-job gate is unavailable. It never installs, flashes, launches, enrolls, or migrates credentials.

## Safe account setup

1. Register a separate certified security key or trusted passkey first.
2. Save account recovery codes offline.
3. Use PepperKey only as a secondary authenticator.
4. Test with disposable or non-critical accounts before wider use.
5. Never remove the independent authenticator based on a build or demo alone.

See `ACCEPTANCE.md` for release gates, `BUILD-STATUS.md` for evidence, `SUPPORT.md` for support boundaries, and `docs/marketing/CLAIMS-GUIDE.md` before publishing claims.
