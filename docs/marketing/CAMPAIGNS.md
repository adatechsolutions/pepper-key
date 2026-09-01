# PepperKey campaign drafts

All copy is draft-only until the release checklist passes. Pair every short ad with a destination that shows the required qualifiers above the fold.

## GitHub and Catalog launch

**Headline:** PepperKey v0.1 — gated USB U2F for Flipper Zero

**Body:** PepperKey starts locked, requires a long local hold to arm and approve, binds approval to the full U2F application identifier, and fails closed on invalid app state. Open-source from Ada Tech Solutions Inc. for official firmware 1.4.3. USB U2F/CTAP1 only; not FIDO Certified; use as a tested secondary authenticator.

## AdaTech article teaser

**Headline:** What it takes to turn a prototype authenticator into a reviewable release

**Body:** We document PepperKey's threat model, fail-closed identity storage, USB lifecycle, pinned build, independent security review, and the limits we refused to market past. Read the source, checks, acceptance plan, and release receipt.

## LinkedIn organic

We built PepperKey as an exercise in honest embedded-security product work: explicit user presence, bounded USB sessions, immutable build evidence, recovery-first testing, and clear non-goals. It is a USB U2F/CTAP1 Flipper Zero app—not a passkey or certified authenticator. Source and acceptance evidence: [launch URL].

## X / short social

PepperKey: open-source USB U2F for Flipper Zero, locked by default and long-hold approved. Pinned build, fail-closed state, honest limits. USB/CTAP1 only; secondary use. [launch URL]

## Search ad test

**Headline 1:** Flipper Zero USB U2F App

**Headline 2:** Open Source, Gated Approval

**Headline 3:** Read The Security Model

**Description:** Explore PepperKey by Ada Tech Solutions. USB U2F/CTAP1 only, built for official firmware 1.4.3. Not FIDO Certified. Test only with a backup authenticator.

## Reddit technical post

**Title:** We hardened Flipper's U2F app into a locked-by-default external FAP—source, threat model, and test gates

**Body outline:** Explain the upstream pin, three remediated findings, exact USB-only scope, pinned build, disposable-account evidence, and remaining limits. Ask for source review and compatibility reports; do not ask for upvotes or lead with a sales offer.

## Video script

1. Show `PEPPERKEY LOCKED` and the version/firmware receipt.
2. Short press: no arm.
3. Long hold: USB session arms.
4. Start disposable registration; show the fingerprint prompt.
5. Long hold approves one request.
6. Show success then automatic lock.
7. End card: “USB U2F/CTAP1 only · not FIDO Certified · secondary authenticator · open-source by Ada Tech Solutions.”

## FAQ responses

**Does it support Bluetooth?** Not in v0.1. Bluetooth would require a separate transport/security design and acceptance program.

**Is it a passkey or FIDO2 key?** No. PepperKey implements legacy U2F/CTAP1 over USB.

**Can it replace my primary security key?** No. Use it only after registering an independent authenticator and saving recovery codes.

**Is it certified or tamper resistant?** No. Flipper Zero is general-purpose, user-flashable hardware, and PepperKey makes no certification or added tamper-resistance claim.
