# PepperKey 1.0 implementation plan

## Delivery rule

Work lands in reviewable slices. A source-complete module remains disabled until the exact artifact passes its security, device, compatibility, failure, and recovery gates. `capabilities.json` changes state only with evidence.

## Foundation — 0.2 alpha

- [x] Preserve v0.1 U2F behavior and artifact evidence.
- [x] Define security domains and immutable derivation labels.
- [x] Add strict Base32 decoding.
- [x] Add HOTP/TOTP portable core and published vectors.
- [x] Add HMAC-SHA-256 challenge core.
- [x] Add versioned vault header and namespace validation.
- [x] Assign CTAP2 model AAGUID.
- [x] Add fail-closed capability/readiness registry.
- [x] Add architecture, protocol, recovery, security, and claims documents.
- [x] Pass pinned uFBT lint/build in GitHub CI for commit `7dde1683a46d9ca3a9b648554504ed9ce77fc305`.
- [x] Pass the separate guarded Pro13 build from an allowed resource window; FAP SHA-256 `2efecbd26dc87623ac2b06aca07f6c56d289759696cf5ebe607af172b65bfaa7`.
- [ ] Independently review this alpha diff.

## FIDO2 transport and parser — 0.3

- [ ] Extract/reuse hardened CTAPHID framing without regressing CTAP1.
- [ ] Add CBOR/CANCEL/KEEPALIVE/capability semantics behind a build-time disabled gate.
- [ ] Implement bounded canonical CBOR and host fuzz harness.
- [ ] Implement GetInfo against the same readiness registry.
- [ ] Add channel, sequence, timeout, lock, cancellation, and malformed-frame tests.

## FIDO2 credentials — 0.4

- [ ] Add ES256 credential creation and assertion.
- [ ] Add non-discoverable wrapped credentials.
- [ ] Add resident credential schema, quotas, selection, and management.
- [ ] Bind approval to RP ID hash, operation, credential, and request digest.
- [ ] Add atomic authenticated persistence and power-loss tests.
- [ ] Add reset/config ceremony and migration/downgrade behavior.

## PIN/UV and extensions — 0.5

- [ ] Implement selected CTAP 2.2 PIN/UV protocols.
- [ ] Decide and document rollback-sensitive retry behavior.
- [ ] Implement `credProtect` and `hmac-secret`/PRF.
- [ ] Evaluate `credBlob` and `largeBlob` after RAM/flash measurements.
- [ ] Run browser, relying-party, OpenSSH, conformance, and negative suites.
- [ ] Enable CTAP2 advertisement only after the complete gate passes.

## OATH vault and UI — 0.6

- [ ] Implement authenticated encrypted OATH records.
- [ ] Add atomic HOTP counters and power-loss recovery.
- [ ] Add bounded URI/manual provisioning.
- [ ] Add profile list, code reveal, countdown, drift warning, delete/reset.
- [ ] Keep host typing and export off by default.
- [ ] Run on-device vectors and 24-hour comparison.

## Challenge-response — 0.7

- [ ] Finalize versioned transcript-bound wire format.
- [ ] Add isolated slot persistence and UI.
- [ ] Add authenticated USB host client with redaction.
- [ ] Add request fingerprint and one-use approval.
- [ ] Run replay/reflection/cross-domain/fault tests.

## Smart card and wireless — separate release tracks

- [ ] Prove safe USB CCID/composite firmware ownership and rollback.
- [ ] Implement/fuzz shared APDU layer.
- [ ] Implement PIV subset and interoperability matrix.
- [ ] Implement OpenPGP card subset and GnuPG matrix.
- [ ] Decide whether firmware distribution remains one product or separate package.
- [ ] Run independent BLE and NFC feasibility/threat-model projects.

## 1.0 qualification

- [ ] Freeze exact feature set; research paths may remain outside 1.0.
- [ ] Independent code/security review with all release blockers closed.
- [ ] Pinned clean build and exact-artifact receipts.
- [ ] Full physical-device, power-loss, corruption, timeout, teardown, and recovery acceptance.
- [ ] Published compatibility matrix with exact versions.
- [ ] Genuine screenshots and redacted demos.
- [ ] Updated privacy, support, claims, commercialization, and end-of-support policies.
- [ ] Catalog/release validation and moderation where applicable.
- [ ] Certification language remains prohibited unless formally granted.
