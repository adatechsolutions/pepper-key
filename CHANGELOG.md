# Changelog

## 0.1.0 — release candidate

- Derived an external FAP from the official Flipper Zero U2F application at firmware 1.4.3.
- Added locked-by-default long-hold arming and per-request approval.
- Added full application-hash/request-type approval binding and pre-prompt handle validation.
- Added one-success lock, request/session timeouts, parser checks, constant-time comparison, and secret zeroization.
- Added fail-closed paired identity provisioning and create-only first-run records.
- Limited new encrypted records to exact 32-byte ciphertext fields.
- Added terminal-error USB teardown through a non-blocking worker/UI event bridge.
- Added pinned build/CI, source checks, security review summary, acceptance gates, support/legal materials, and Catalog preparation.

The version remains a release candidate until physical device, disposable-account, fault, recovery, screenshot, immutable commit, and Catalog moderation gates pass.
