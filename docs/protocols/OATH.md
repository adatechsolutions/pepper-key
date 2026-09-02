# OATH HOTP and TOTP

## Implemented portable core

`app/pepper_otp.c` implements:

- HOTP dynamic truncation and counter encoding from RFC 4226,
- TOTP time-step derivation from RFC 6238,
- HMAC-SHA-1, HMAC-SHA-256, and HMAC-SHA-512,
- 6, 7, or 8 digits,
- caller-provided epoch and period,
- zero-padded display formatting,
- secret/digest/moving-factor validation and zeroization.

`app/pepper_base32.c` accepts uppercase/lowercase RFC 4648 Base32, optional visual spaces/hyphens, and optional canonical padding. It rejects invalid characters, data after padding, impossible symbol counts, insufficient output, and non-zero residual bits.

Native tests cover RFC 4226 counters 0–9 and RFC 6238 time 59 for all three hash algorithms. Published vectors contain no live secret.

## Device feature still required

The core is not an on-device authenticator until all of the following exist:

- isolated encrypted/authenticated OATH vault,
- bounded profile schema and count,
- atomic HOTP counter persistence before display/typing,
- safe add/edit/delete/reset UI,
- issuer/account labels with length limits and control-character rejection,
- manual or authenticated provisioning that does not leave plaintext files,
- RTC validation, drift indicator, time-change warning, and failure behavior,
- explicit reveal and optional typing policy,
- clipboard/log/notification redaction,
- same-device backup and deletion tests.

## Proposed profile schema

| Field | Constraint |
|---|---|
| Type | HOTP or TOTP |
| Algorithm | SHA-1, SHA-256, or SHA-512 |
| Digits | 6–8 |
| Secret | 10–64 decoded bytes; warning below 20 bytes |
| Issuer | 32 visible bytes maximum |
| Account | 48 visible bytes maximum |
| Period | 15–120 seconds for TOTP |
| Counter | 64-bit for HOTP |
| Generation | Non-zero monotonic best-effort record generation |

Initial profile quota is 32, subject to measured RAM/flash cost.

## Provisioning policy

Preferred order:

1. On-device entry for short/test seeds.
2. Authenticated USB provisioning session with a one-time local confirmation and displayed import summary.
3. QR import only if a trusted camera module and parser are separately reviewed.

Plaintext SD import files are not an accepted production path because flash deletion may not erase prior blocks. URI parsing must reject unknown required parameters, duplicate fields, oversized percent-decoding, unsupported algorithms, invalid periods/digits, and empty labels/secrets.

## Display and typing

Codes are revealed only after local selection and approval. The screen shows issuer/account plus remaining validity; it never shows the seed. Automatic keyboard typing is off by default because it exposes the code to the focused host and can type into the wrong window. If implemented, it requires a second confirmation and clear host-warning UI.

## Backup and export

Live seed export is disabled by default. A future export must use the recovery-domain format, authenticated encryption, explicit per-profile selection, a locally confirmed ceremony, and a warning that exported seeds can clone the authenticator. Screenshots, logs, issues, and diagnostics must not contain codes or seeds.

## Release gate

- Portable core vectors and invalid-input tests.
- On-device vectors using synthetic profiles.
- RTC forward/backward/time-zone changes.
- Counter-write failure and power loss at every HOTP transition.
- Vault corruption, missing/partial records, wrong device, quota, deletion, and reset.
- 24-hour TOTP comparison with two independent implementations.
- Provisioning parser fuzzing and redaction review.
- Exact-artifact physical acceptance and recovery rehearsal.

## Primary specifications

- RFC 4226 HOTP: <https://www.rfc-editor.org/rfc/rfc4226.html>
- RFC 6238 TOTP: <https://www.rfc-editor.org/rfc/rfc6238.html>
- RFC 4648 Base-N encodings: <https://www.rfc-editor.org/rfc/rfc4648.html>
