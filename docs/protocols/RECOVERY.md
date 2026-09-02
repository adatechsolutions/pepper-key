# Recovery and portability

## Safety objective

Recovery must not silently turn every PepperKey into a cloneable bearer archive. Availability and device binding are competing goals, so recovery is explicit per domain and never implied by copying the whole app-data directory.

## Current device-tested behavior

The v0.1 U2F records are encrypted with a device-unique enclave key. Same-device restore remains an open acceptance gate; the design does not create a replacement authenticator on another device.

The owner must retain an independent authenticator and offline account recovery codes. PepperKey is not the sole recovery path.

## Recovery classes

### Same-device protected backup

- Preserve opaque encrypted records without opening or printing them.
- Record device identity privately, file count/size, backup time, firmware/app versions, and checksums.
- Protect the backup destination with full-disk encryption and least access.
- Restore only during a controlled ceremony after preserving the failed state.
- Verify read-back and authenticate to a disposable account before trusting recovery.

### Portable encrypted export

This remains unimplemented. If approved, the format requires:

- version, suite/domain, source model, creation time, export generation, salt, nonce, KDF parameters, ciphertext length, and authenticated metadata,
- memory-hard password KDF supported by the chosen host/device split,
- modern authenticated encryption,
- independent random recovery secret or threshold shares where justified,
- explicit domain/credential selection,
- clone and rollback warnings,
- import quotas, duplicate handling, and destructive cleanup guidance.

The export password/secret may never be logged, passed through shell history, embedded in a QR screenshot, committed, or stored beside the export.

### Relying-party recovery

Account recovery codes and a second certified authenticator remain the preferred recovery method. PepperKey documentation must teach removal/re-enrollment rather than promising universal key portability.

## Domain policy

| Domain | Default portability |
|---|---|
| Legacy U2F | Same-device only |
| FIDO2 non-discoverable | Re-enroll through relying party |
| FIDO2 discoverable | Same-device only unless a future reviewed export explicitly supports it |
| OATH | Export disabled; future per-profile encrypted export possible |
| Challenge-response | Export disabled; verifier-side rotation preferred |
| PIV/OpenPGP | Export disabled for on-device-generated private keys |
| Recovery metadata | Portable only inside the authenticated recovery package |

## Restore ceremony

1. Confirm the independent authenticator/recovery path.
2. Record exact source/target device, firmware, PepperKey version, and artifact digest.
3. Preserve the target before-state.
4. Validate package structure, sizes, KDF bounds, authentication tag, domain, schema, and generation before any write.
5. Display an import summary and clone/rollback warning.
6. Require a long local confirmation.
7. Write to a new path, read back, authenticate/decrypt, and atomically activate.
8. Keep the prior valid generation until the new one is proven, then retire it deliberately.
9. Run disposable-account authentication and document result.

Any failure leaves the old state active and produces no automatic reset or replacement.

## Release gate

- Format and KDF/AEAD selection reviewed independently.
- Published synthetic vectors and cross-platform reference reader.
- Wrong-password, modified-header/ciphertext/tag, truncation, extension, duplicate, oversized-KDF, downgrade, rollback, clone, and power-loss cases.
- Same-device restore and replacement-device policy tested separately.
- Documentation and UI make clone/rollback consequences unmistakable.
- No live recovery package enters CI, issues, logs, screenshots, or release assets.
