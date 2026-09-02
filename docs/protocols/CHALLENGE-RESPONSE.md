# HMAC challenge-response

## Core

`pepper_challenge_response` computes a 32-byte HMAC-SHA-256 response for a non-empty challenge up to 1,024 bytes. Keys come from the `challenge` vault namespace and never reuse FIDO, OATH, smart-card, or recovery material.

The core is transport-neutral and not exposed over USB. This prevents an unfinished host protocol from becoming an unauthenticated signing oracle.

## Proposed host protocol

Each request must include:

- protocol version,
- operation code,
- slot identifier,
- caller/application context,
- random 32-byte challenge,
- optional bounded application payload,
- transport transcript binding.

The device parses and hashes the complete request, displays the slot/caller plus a short transcript fingerprint, and requires a long local hold. Approval is one-use and expires after ten seconds. The response covers the version, operation, slot, caller context, challenge, payload hash, and transport binding—not only attacker-chosen bytes.

## Slots

Initial maximum: eight. Each slot has a random key, user-visible label, creation generation, enabled operation set, and export policy. Slot deletion and replacement are destructive local ceremonies. No slot may be created or overwritten due to a load/decryption error.

## Host client

A future `pepperkey challenge` client must:

- use framed and length-bounded messages,
- verify protocol version and response length,
- set explicit timeouts and cancellation,
- never print keys or full responses by default,
- avoid shell interpolation of challenge material,
- display the device fingerprint for operator comparison,
- return non-zero on denial, timeout, disconnect, parse failure, or mismatch.

## Anti-replay

The relying application supplies an unpredictable challenge and tracks consumption. PepperKey cannot prevent a malicious verifier from replaying its own requests. Timestamp-only challenges are insufficient. Protocol documentation must define domain strings so a response for one application cannot be accepted by another.

## Release gate

- Versioned wire specification and independent parser implementation.
- Mutual transcript tests and published synthetic vectors.
- Replay, reflection, cross-slot, cross-application, truncation, extension, duplicate, oversized, cancellation, disconnect, and timeout cases.
- Request-bound local approval and UI fingerprint tests.
- Slot persistence, corruption, deletion, reset, backup, and power-loss tests.
- Host-client redaction/security review.
- Exact-device compatibility matrix.
