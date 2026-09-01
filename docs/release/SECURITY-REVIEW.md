# PepperKey v0.1 security review summary

## Scope and method

The complete pre-release source snapshot received a multi-pass static security review covering USB framing and parser bounds, user-presence binding, identity/counter persistence, enclave use, secret lifetime, session teardown, build provenance, and release boundaries. A separate pre-patch investigation traced each confirmed finding, and a fresh post-patch reviewer searched for bypasses and regressions.

This is source review evidence, not FIDO certification, penetration testing, hardware evaluation, formal verification, or a third-party professional audit.

## Confirmed findings and disposition

### Unwritten encrypted-record suffix — fixed

Three writers encrypted 32 bytes but serialized a 48-byte buffer, allowing 16 unwritten stack bytes into the record. New records now contain exactly 32 ciphertext bytes, the counter size is compile-time asserted, and the unchanged stock Type 0 certificate envelope is an explicit read-only legacy exception.

### Destructive state-load fallback — fixed

Missing, malformed, I/O, and decryption failures previously collapsed into one boolean and could silently replace the credential root or reset the counter. Load results are now typed. Provisioning occurs only when both identity records are conclusively absent; every mixed or error state fails locked without replacement/reset.

### Terminal error left USB/session keys active — fixed

The original data-error path stopped timers but omitted full teardown. The first repair used the common cleanup, but bypass review found a queue-saturation deadlock between the UI and HID worker. Worker notifications now use coalescing event flags on the UI event loop, DataError has priority, and teardown can join the worker, restore USB, and zeroize/free session state without waiting on a full GUI queue.

## Verification completed

- Stock asset digests remain pinned and unchanged.
- Focused source-policy checks pass.
- uFBT 0.2.6 lint passes.
- The external FAP builds with the official SDK 1.4.3 for target F7 / API 87.1.
- The fresh reviewer found no remaining source bypass for the three findings.

## Verification still required

Physical device fault injection, USB restoration under notification flooding, browser/relying-party compatibility, recovery, and genuine screenshot acceptance remain open. See `ACCEPTANCE.md`. Until those gates pass, PepperKey is a built and reviewed candidate, not an accepted release.
