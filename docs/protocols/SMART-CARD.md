# PIV and OpenPGP smart-card research

## Current decision

PIV and OpenPGP card support are research tracks, not external-FAP alpha features. They require a USB CCID or standards-compatible composite interface and therefore may require a custom firmware component instead of the U2F-only external app API.

No smart-card capability is advertised and no live key is provisioned until the USB ownership, firmware distribution, update, rollback, and interoperability boundaries are resolved.

## Shared CCID layer

The shared layer must provide:

- USB descriptors and CCID message framing,
- one bounded APDU parser,
- command chaining and extended-length policy,
- deterministic status words,
- cancellation/timeouts/disconnect cleanup,
- exclusive USB owner arbitration,
- buffer zeroization,
- host compatibility with macOS, Windows, Linux, pcsc-lite, and selected middleware.

Fuzzing and sanitizer tests run against a host build before device exposure.

## PIV profile

Target the appropriate NIST SP 800-73-5 PIV card interfaces with a documented subset. Candidate capabilities:

- PIV authentication,
- digital signature,
- key management/decryption,
- card authentication,
- certificate/object storage,
- PIN verify/change/unblock/reset policy,
- on-device generation and controlled certificate import.

Private keys are isolated by role. Signing and decryption require explicit local approval in addition to any standard PIN requirement unless compatibility evidence demands a narrower policy. Administrative key handling, attestation, and management-key algorithms require a separate provisioning design.

## OpenPGP card profile

Candidate capabilities:

- signature key,
- decryption key,
- authentication key,
- on-device key generation,
- public-key/certificate metadata,
- user/admin PIN flows,
- GnuPG and SSH-via-GPG-agent interoperability where supported.

The three private-key roles remain isolated. Key import/export is off by default. Touch policy is enforced locally and documented independently from PIN policy.

## Hardware limitations

The Flipper is general-purpose user-flashable hardware and cannot honestly promise smart-card secure-element resistance. Application storage rollback can affect retry counters and metadata. Certificate/object sizes and cryptographic algorithms must fit measured flash/RAM/latency budgets. Unsupported algorithms are rejected, not emulated with weaker substitutions.

## Release gate

- Firmware/API feasibility proof and rollback path.
- Signed or checksum-bound firmware distribution plan.
- CCID descriptor and parser review/fuzzing.
- NIST/GnuPG interoperability matrices.
- PIN/retry/lock/reset/corruption/power-loss tests.
- Key generation, import, deletion, role isolation, and approval tests.
- Host middleware compatibility and disconnect recovery.
- Independent security review and claims update.

## Primary specifications

- NIST SP 800-73-5 PIV Part 2: <https://csrc.nist.gov/pubs/sp/800/73/pt2/5/final>
- GnuPG OpenPGP card specifications: <https://gnupg.org/ftp/specs/>
