# PepperKey release checklist

Every checked item needs a dated receipt. A later code, manifest, SDK, asset, or device-state change invalidates dependent evidence.

## Source and legal

- [ ] Canonical repository is public under `adatechsolutions/pepper-key`.
- [ ] Default branch and release permissions are restricted to AdaTech maintainers.
- [ ] `LICENSE`, `NOTICE.md`, `UPSTREAM.md`, source changes, and corresponding source are present.
- [ ] PepperKey name receives trademark/confusion clearance before paid promotion.
- [ ] Claims comply with `docs/marketing/CLAIMS-GUIDE.md`; no certification logo or implied endorsement appears.
- [ ] Privacy/terms review confirms the app itself collects no telemetry and identifies any separate website analytics.

## Security and build

- [ ] `tests/check-source.sh` passes.
- [ ] `tests/check-security-fixes.sh` passes.
- [ ] uFBT lint passes.
- [ ] CI passes from the official SDK 1.4.3 archive and pinned digest.
- [ ] Independent review finds no unresolved release-blocking issue.
- [ ] Guarded AdaTech build is from the immutable release commit.
- [ ] FAP size and SHA-256 are recorded in the release receipt.
- [ ] Stock asset hashes remain unchanged.

## Physical device and compatibility

- [ ] Intended Pepper identity, official firmware/API, battery, microSD, temperature, and USB ownership are read-only verified.
- [ ] Before-state backup and rollback path exist.
- [ ] Clean first-run provisioning passes.
- [ ] Missing-one-file and corrupt-state cases fail closed with zero replacement/reset.
- [ ] Notification flood plus terminal DataError restores USB and locks within a bounded time.
- [ ] Short press cannot arm or approve; long hold is required.
- [ ] One-success lock, request timeout, and two-minute session timeout pass.
- [ ] Counter advances across power cycle and write failure returns no signature.
- [ ] Disposable-account registration and authentication pass on every claimed host/browser/service.
- [ ] Independent backup authenticator and recovery codes remain valid.
- [ ] Same-device recovery passes; cross-device app-data restore is not claimed.

## Catalog and release

- [ ] Genuine screenshot matches the accepted build and contains no identifiers.
- [ ] Catalog manifest contains the exact 40-character AdaTech source commit.
- [ ] Official catalog bundle validator passes.
- [ ] One-app Catalog pull request is opened with accurate AI disclosure.
- [ ] GitHub release notes repeat USB-only/U2F-only/secondary-authenticator limits.
- [ ] Release attaches FAP, SHA-256 file, source link, acceptance receipt, and rollback notes.
- [ ] No paid campaign starts before a named owner approves budget, audience, destination, measurement, and stop conditions.

## Stop conditions

Stop publication for an unresolved security finding, pin mismatch, stale artifact hash, missing physical acceptance, incompatible claimed platform, missing genuine screenshot, unclear rights/marks, or absent account recovery path.
