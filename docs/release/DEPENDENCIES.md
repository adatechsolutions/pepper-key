# Build and runtime dependencies

## Pinned build inputs

- uFBT 0.2.6.
- Official Flipper F7 SDK 1.4.3 / API 87.1.
- Official SDK archive SHA-256 `2e89e70c6b5770440cbf02f2ca01a2f8804e05ddb77f66afdd23ed2584740c7f`.
- Upstream firmware source commit `8622f1a2b83d8f4918dd5fa3f43de963f6d6f819` for provenance review.

CI downloads the official SDK archive, verifies its digest, installs it locally, checks the resolved version/target, and then builds.

## Runtime interfaces

PepperKey imports APIs exposed by the pinned Flipper SDK for GUI/input, timers, notification, USB HID U2F, storage/FlipperFormat, randomness, enclave-backed crypto, and MbedTLS secp256r1 operations. It contains no third-party network, analytics, advertising, or account SDK.

Any SDK/firmware upgrade is a new compatibility and security change. It requires source diff review, fresh import validation, rebuild, physical device acceptance, and an updated release receipt.
