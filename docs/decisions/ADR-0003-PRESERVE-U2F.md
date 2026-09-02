# ADR-0003: Preserve the device-tested U2F identity

Status: accepted architecture decision

## Context

PepperKey v0.1 has a device-bound identity already enrolled in a disposable WebAuthn test. Refactoring its root, paths, or key-handle derivation could invalidate registrations or create unsafe migration behavior.

## Decision

The v0.1 U2F record paths, key-handle derivation, stock attestation assets, counter format, arming/approval policy, and fail-closed behavior remain the legacy-FIDO domain. New modules use new paths and roots. Migration is opt-in, backup-first, versioned, and separately accepted.

## Consequences

- Existing U2F registrations remain stable unless a deliberate breaking release says otherwise.
- New architecture cannot “clean up” the old record format casually.
- The repository carries a compatibility boundary and regression suite.
- Users can roll back to the device-tested v0.1 artifact before new enrollment.
