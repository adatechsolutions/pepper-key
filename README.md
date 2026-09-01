# PepperKey

PepperKey is a hardened USB U2F/CTAP1 application for Flipper Zero. It is derived from the official Flipper Zero U2F application and targets official stock firmware 1.4.3, target F7, API 87.1.

It is maintained by [Ada Tech Solutions Inc.](https://adatechsolutions.ca) in the canonical repository `adatechsolutions/pepper-key`.

PepperKey is not a replacement for a purpose-built certified authenticator, a FIDO2/CTAP2 authenticator, a passkey provider, or a FIDO-certified product. Its purpose is to make the strongest honest U2F key possible within Flipper Zero's existing hardware and official application framework.

## Security behavior

- Starts locked. Holding OK explicitly arms a two-minute USB session.
- Requires a long OK hold for every registration and sign-in, including legacy requests that ask not to enforce presence.
- Shows the first eight hexadecimal characters of the site's 32-byte U2F application hash. This is a continuity hint, not a human-readable or collision-resistant site identity.
- Binds approval to the complete 32-byte application hash captured when the prompt appeared.
- Rejects unknown key handles and U2F check-only probes before displaying an approval prompt.
- Locks itself three seconds after one successful registration or authentication.
- Uses constant-time key-handle verification and clears transient key material from RAM.
- Validates command and key-handle lengths before parsing.
- Persists the next authentication counter before sending a response, preventing reuse after a storage-write failure.
- Creates identity records only when both are conclusively absent; corrupt, incomplete, or unreadable state fails closed without replacing the credential root or resetting the counter.
- Serializes only the 32 ciphertext bytes produced by the enclave API. The unchanged stock attestation asset is read through an explicit legacy envelope path.
- Disarms, restores the prior USB mode, and zeroizes session keys after a terminal data error.
- Keeps its identity separate from the stock U2F app.

## Storage

The packaged attestation assets are extracted by Flipper's FAP loader under the app-specific assets directory. PepperKey creates its mutable identity only after the user explicitly arms it:

- `/ext/apps_data/pepper_key/key.u2f`
- `/ext/apps_data/pepper_key/cnt.u2f`

The device key and counter are encrypted with the Flipper Zero's device-unique enclave key. Do not inspect, publish, edit, delete, or casually restore these files. A same-device backup may recover an SD-card failure, but it cannot create a replacement authenticator on another Flipper.

PepperKey never silently regenerates incomplete or unreadable app data. If both files are absent, it treats the next explicit arm as a clean first run. Deleting both files therefore creates a new authenticator identity, and accounts registered to the old identity may become inaccessible. If only one file is missing or either record is invalid, PepperKey remains locked so the owner can recover a known-good same-device backup or make an explicit destructive reset decision.

## Safe account setup

1. Register a separate FIDO2 security key or trusted passkey first.
2. Save the account's recovery codes offline.
3. Register PepperKey only as a secondary authenticator.
4. Test registration and sign-in on a non-critical account before wider use.
5. Never remove the primary authenticator until PepperKey has passed the complete acceptance plan.

See `ACCEPTANCE.md` for the build, installation, test, and recovery gates. See `SECURITY.md` for the threat model and residual risks.

## Current status

The source candidate has passed focused source checks, uFBT lint, an independent security review/remediation cycle, and a pinned external-FAP build. Installation, physical device testing, disposable-account compatibility, recovery testing, a genuine screenshot, immutable release binding, and Apps Catalog moderation remain separate gates. A successful build alone does not establish that a browser, service, or signed-in account accepts PepperKey.

See `BUILD-STATUS.md` for the exact current evidence, `SUPPORT.md` for support boundaries, and `docs/marketing/CLAIMS-GUIDE.md` before publishing product claims.
