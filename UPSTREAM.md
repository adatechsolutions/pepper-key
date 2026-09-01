# Upstream provenance

PepperKey is a derivative of the official Flipper Devices U2F application:

- Repository: `https://github.com/flipperdevices/flipperzero-firmware`
- Release/tag: `1.4.3`
- Commit: `8622f1a2b83d8f4918dd5fa3f43de963f6d6f819`
- Original path: `applications/main/u2f`
- License: GNU General Public License v3.0; the copied upstream `LICENSE` is included.

The original certificate assets are redistributed unchanged inside the FAP file-assets directory. Mutable PepperKey identity files are moved to the application-specific data alias and do not share state with the stock U2F app.

Pinned SHA-256 checksums for the unchanged upstream assets:

- `cert.der`: `6cfb7adf2808b887c7e04afb96242e904f28f940993a4fb1ad07150f327d9ffe`
- `cert_key.u2f`: `9eadb4088b517fc1f24514bbee2105414747509155e622dcf979fcb37bde73ae`

Material changes include explicit arming, long-hold and one-shot approval policy, full application-hash approval binding, short display fingerprint, pre-prompt key-handle validation, strict presence enforcement, constant-time comparison, parser bounds checks, counter write-before-response, fail-closed paired state provisioning, exact-size encrypted records, terminal-error teardown, secret zeroization, corrected HID unlock handling, app-specific assets/data, and documented recovery boundaries.

The inherited Type 0 `cert_key.u2f` remains byte-for-byte unchanged for stock compatibility. Its 48-byte `Data` envelope is treated as a legacy input only; PepperKey consumes the first 32 ciphertext bytes and writes 32-byte `Data` fields for every new encrypted record.
