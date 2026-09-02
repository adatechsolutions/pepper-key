# PepperKey 1.0 architecture

## Outcome

PepperKey 1.0 is one owner-facing authenticator suite with isolated protocol domains. The product may share navigation, notifications, storage plumbing, and release tooling, but it may not share credentials, approval grants, reset effects, counters, or unlabeled cryptographic roots between domains.

The device-tested v0.1 U2F application remains the fallback and migration anchor. The 0.2 alpha builds portable foundations beside it. No alpha module is advertised over USB until its complete acceptance gate passes.

## Component map

```text
PepperKey shell
├── Legacy FIDO domain
│   └── device-tested U2F/CTAP1 engine and records
├── FIDO2 domain
│   ├── CTAPHID/CBOR transport
│   ├── CTAP2 command state machine
│   ├── ES256 credential engine
│   ├── PIN/UV and retry state
│   └── resident credential store and extensions
├── OATH domain
│   ├── strict Base32 provisioning parser
│   ├── HOTP/TOTP core
│   └── encrypted profiles and atomic counters
├── Challenge domain
│   ├── HMAC-SHA-256 slots
│   └── authenticated host transport
├── Smart-card domain
│   ├── USB CCID/composite firmware
│   ├── PIV application
│   └── OpenPGP card application
└── Recovery domain
    ├── same-device backup
    └── optional portable encrypted export
```

Bluetooth and NFC are transports around individual domains, not a universal route into the whole suite.

## Cryptographic domain separation

`pepper_vault_derive_namespace_key` derives a 32-byte domain key from a root using HMAC-SHA-256 and one of these fixed contexts:

- `PepperKey/v1/vault/fido2`
- `PepperKey/v1/vault/oath`
- `PepperKey/v1/vault/challenge`
- `PepperKey/v1/vault/smart-card`
- `PepperKey/v1/vault/recovery`

The namespace byte is appended to the context. Domain labels are immutable protocol inputs. Changing a label creates a new key domain and therefore requires an explicit migration.

Legacy U2F does not use this derivation and retains its device-tested v0.1 root. This avoids silently changing already-enrolled credentials.

## Storage contract

Every new store begins with a 32-byte `PepperVaultHeader` containing:

- magic `PVLT` (`0x50564C54`),
- schema version,
- namespace identifier,
- flags,
- non-zero generation,
- bounded payload size,
- 16-byte salt.

The header validator rejects wrong magic/version/namespace, unknown flags, generation zero, empty payloads, and oversized payloads. This header is metadata, not authentication. The persistence layer must encrypt and authenticate the complete header and payload, use fresh nonces, perform an atomic replace, and retain no plaintext rollback copy.

Each domain needs independent quotas. Exhaustion must return a stable error without deleting or overwriting another record.

## Approval contract

An approval grant contains at least:

- domain and operation,
- complete request binding or hash,
- relying-party/account/slot display cue,
- creation and expiry time,
- one-use state.

A grant cannot move between domains or requests. Disconnect, cancellation, timeout, lock, application exit, terminal error, firmware transition, or successful use destroys it. A background or wireless request cannot reuse a prior USB approval.

## Capability advertisement

`capabilities.json` is the canonical product state. Runtime flags are even stricter: an implementation may exist in source while the USB or radio advertisement remains false. CTAP2 is the first enforced example—`pepper_ctap2_readiness().usb_cbor_transport` is false, so `authenticatorGetInfo` and CTAPHID_CBOR are not exposed.

Statuses mean:

- `accepted`: source, artifact, device, compatibility, fault, and recovery evidence exists.
- `core-ready`: bounded portable core passes published vectors, but the device workflow does not.
- `implementation`: active code/specification work.
- `research`: architecture/dependency work only.
- `blocked`: current boundary cannot support the feature safely.

## Memory and performance budgets

Flipper Zero provides a 64 MHz application core, shared 1 MB flash, and shared 256 KB SRAM. The suite therefore uses bounded static structures, streaming parsers, credential quotas, and explicit buffer clearing. No parser accepts host-declared unbounded allocation.

Proposed initial limits, subject to measurement:

| Resource | Initial ceiling |
|---|---:|
| CTAP HID message | Existing HID transport maximum, never host-expanded |
| Resident credentials | 25 |
| OATH profiles | 32 |
| OATH secret | 64 bytes decoded |
| Challenge slots | 8 |
| Challenge message | 1,024 bytes |
| Smart-card certificate object | 2,048 bytes |
| Approval lifetime | 10 seconds |
| Armed session | 120 seconds |

Ceilings must be reduced if stack, heap, latency, or flash-wear evidence requires it.

## Failure model

Any malformed record, decryption/authentication failure, version mismatch, partial state, counter write failure, protocol parser error, impossible state transition, or USB/radio teardown failure locks only the affected domain and produces no credential operation. It must not provision a replacement identity automatically.

Power loss must be tested at every persistent transition. For monotonic-sensitive state where hardware cannot guarantee rollback prevention, documentation and UI must disclose the residual risk.

## Update and migration

- v0.1 U2F files are never automatically moved.
- New schemas use new paths and versions.
- Read-only inspection precedes migration.
- Migration requires explicit local confirmation, verified backup, dry-run validation, and post-write read-back.
- A failed migration leaves the old accepted state usable.
- Downgrade must not cause a newer record to be silently misread.

## Release units

A release unit is the exact source commit, SDK/toolchain versions, built FAP/firmware digest, hardware/firmware identity, compatibility matrix, acceptance receipt, recovery result, and claims text. A source merge, unit test, dashboard color, mounted volume, or successful build is not end-to-end evidence.
