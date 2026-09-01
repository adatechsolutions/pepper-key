# PepperKey security model

## Protected workflow

PepperKey protects U2F registration and authentication over a directly connected USB host. Its private credential derivation key and authentication counter are stored as encrypted app data and decrypted only on the Flipper Zero while an armed session is active.

## Defensive changes from the official 1.4.3 U2F app

| Risk | PepperKey control |
|---|---|
| Accidental app launch or unattended connection | Explicit long-hold arming and a two-minute maximum session |
| Accidental approval | Long-hold approval rather than a short press |
| Approval consumed by a different site request | Full 32-byte application-hash binding |
| Unknown key-handle prompt spam | Handle validation before prompting |
| Legacy no-presence authentication | Presence is always required for signing |
| Key-handle comparison timing leakage | Constant-time comparison |
| Short or malformed messages | Command, request, and handle-length checks |
| Counter reuse after failed persistence | Counter is written before a signed response is returned |
| Corrupt state silently replacing identity or resetting counter | Typed record-state checks; provisioning only when both records are conclusively absent |
| Partial first-run provisioning | Create-only record writes and rollback limited to files created by that provisioning attempt |
| Encrypted records exposing unwritten stack bytes | Exact 32-byte ciphertext fields for all newly written records |
| Terminal data error leaving USB and keys active | Shared teardown stops HID, restores USB, and zeroizes the session before showing the locked error |
| Sensitive stack and heap remnants | Volatile zeroization of transient and long-lived key buffers |
| USB channel lock persisting indefinitely | Correct unlock-event wait mask and ten-second protocol limit |
| Stock U2F app bypassing PepperKey policy | Separate app ID, asset directory, device key, and counter |

## Explicit non-goals and residual risks

- This is U2F/CTAP1 only. It does not implement FIDO2/CTAP2, discoverable credentials, authenticator PINs, resident passkeys, credProtect, large blobs, enterprise attestation, or NFC/BLE transport.
- The eight-character display fingerprint is only a short visual continuity cue. A user must not treat it as proof of a domain name.
- Flipper Zero is a general-purpose, user-flashable device rather than a purpose-built certified authenticator. PepperKey adds no certification or tamper-resistance claim.
- The official stock attestation certificate and encrypted certificate key are inherited byte-for-byte from Flipper firmware 1.4.3. Its legacy 48-byte envelope is accepted only for the stock Type 0 asset; PepperKey decrypts the 32-byte ciphertext and never writes that legacy shape for new records.
- A malicious host can send requests, disconnect USB, or decline a response. It should not receive a signature without an approved, matching request. The v0.1 candidate completed an independent multi-pass static review and remediation cycle, but it has not received a third-party professional audit or certification.
- A malicious or replaced firmware image can undermine application-level controls.
- The authentication counter is encrypted but is not backed by a hardware monotonic counter. Restoring an old same-device SD backup can roll it back; relying parties may detect this as possible cloning.
- Loss or destruction of the Flipper Zero can make its registered credentials unrecoverable. An app-data backup cannot be moved to a replacement device because encryption is bound to the original Flipper Zero's device-unique enclave key.
- Browser and relying-party compatibility varies, especially on newer macOS versions that may reject older U2F behavior.

## Secrets and diagnostics

Never commit or attach generated `key.u2f`, `cnt.u2f`, device identifiers, account recovery codes, captured authentication traffic, or copied app-data directories. Logs must not include application hashes, key handles, challenges, device keys, certificate keys, or account identifiers.

## Reporting a vulnerability

Use GitHub private vulnerability reporting in `adatechsolutions/pepper-key`. Do not open a public issue. Provide the affected commit/version, a redacted reproduction, security impact, and suggested mitigation if known. Never include live credentials, decrypted key material, app-data files, device identifiers, or account details.

## Review requirement

Treat PepperKey as experimental until the pinned build passes, source changes receive an independent review, and the device/browser acceptance plan passes using a disposable test account. Do not make it the sole authenticator for an important account.
