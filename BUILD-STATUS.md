# PepperKey build status

Status: `BUILD_PASS / HOLD_DEVICE_ACCEPTANCE`

Date: 2026-09-01

## Completed

- Forked the official Flipper Zero U2F application from release 1.4.3, commit `8622f1a2b83d8f4918dd5fa3f43de963f6d6f819`.
- Implemented PepperKey's isolated identity, arming, approval binding, one-shot session, parser, counter, zeroization, and HID hardening.
- Verified the stock certificate assets are byte-identical to the pinned upstream copies.
- `ufbt format` completed successfully.
- `ufbt lint` completed successfully.
- `tests/check-source.sh` completed successfully.
- Converted the app manifest to a catalog-style external FAP, added version 0.1 metadata and target F7, and replaced the host-specific provenance test with pinned SHA-256 checks.
- Added catalog-safe description and changelog files plus a manifest and pull-request draft.
- Confirmed through the official release source that 1.4.3 remains the latest stock release and that the official U2F path has no commits after that tag.
- Completed an independent security scan, remediated all three confirmed findings, and passed a fresh bypass/regression review.
- Corrected internal-firmware-only dependencies so the app builds as a genuine external FAP.
- Built `app/dist/pepper_key.fap` with uFBT 0.2.6, SDK 1.4.3, target F7, API 87.1.
- Recorded final candidate SHA-256 `e22b2cd543ad7bc02d4dd55b3b31770a8900a08065c585e1f7a7fb9ea62b445a` (69,096 bytes).

## Held

No upload, launch, installation, enrollment, physical device test, disposable-account test, recovery test, or notification-flood fault injection has occurred.

No genuine tested screenshot is present, no Apps Catalog bundle validation ran, and no catalog pull request was opened. Public source publication and release tagging must record an immutable AdaTech commit and rebuild that exact source before the Catalog submission gate.

## Rebuild command

Run from the repository root:

```sh
./scripts/build-release.sh
```

The script fails closed on resource, serialization, source-policy, linter, or pin mismatch. It builds only; it never launches or installs the app.

## Current artifact

```text
app/dist/pepper_key.fap
```

Treat that artifact as unaccepted until every relevant gate in `ACCEPTANCE.md` passes. A later source change invalidates the hash above and requires a new guarded build.
