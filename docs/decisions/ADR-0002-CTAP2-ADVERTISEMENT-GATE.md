# ADR-0002: Fail-closed CTAP2 advertisement

Status: accepted architecture decision

## Context

Browsers choose authenticator behavior from CTAPHID capability bits and GetInfo. Advertising a partial implementation can route real passkey ceremonies into incomplete security state.

## Decision

CTAP2 USB CBOR capability remains false until transport, required commands, PIN/UV obligations, storage, approval, fault handling, conformance, and device/browser acceptance pass together. GetInfo code may be developed and tested internally but is not exposed early.

## Consequences

- Alpha builds remain honest U2F devices to the host.
- Incomplete CTAP2 code cannot receive live credentials through normal routing.
- Integration is enabled later in one deliberate reviewed change.
- End-to-end discovery testing happens later than component testing.
