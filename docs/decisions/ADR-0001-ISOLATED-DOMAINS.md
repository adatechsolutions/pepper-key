# ADR-0001: Isolated authenticator domains

Status: accepted architecture decision

## Context

An all-in-one authenticator can become one large failure domain if passkeys, OTP seeds, challenge keys, smart-card keys, approvals, resets, and recovery share one root or store.

## Decision

PepperKey presents one product shell but uses separate labeled derivation roots, records, quotas, parsers, approval grants, resets, and release evidence for legacy FIDO, FIDO2, OATH, challenge-response, smart-card, and recovery.

The device-tested v0.1 U2F identity remains outside the new derivation tree. No automatic migration occurs.

## Consequences

- Compromise or corruption of one parser/store has a smaller intended blast radius.
- Reset and backup UX is more explicit.
- Cross-domain reuse can be tested mechanically.
- Code and storage overhead increase.
- “Reset PepperKey” must enumerate affected domains instead of hiding a global wipe.
- A common device/firmware compromise remains able to undermine all application domains.
