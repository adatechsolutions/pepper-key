# Safe provisioning operations

## Global rules

- Provision only after source, artifact, device, and recovery gates for the capability pass.
- Keep a separate authenticator and offline account recovery codes.
- Use disposable accounts and synthetic secrets until release acceptance.
- Close qFlipper, RPC, Lab, serial tools, and other mutable owners before device writes.
- Resolve the exact device/firmware/artifact and preserve a before-state backup.
- Never pass a secret on a shell command line, paste it into logs/issues/chat, or save it in the repository.
- Read back and validate every persistent write before enrolling an account.

## FIDO enrollment

1. Confirm the other authenticator/recovery path works.
2. Verify PepperKey capability/status and exact artifact digest.
3. Arm the intended domain locally.
4. Confirm the browser/client and relying-party identity.
5. Start registration and compare the displayed request cue.
6. Approve once with the required local hold.
7. Lock/re-arm and test authentication.
8. Reboot and repeat authentication.
9. Record only redacted compatibility evidence.

Never remove the prior authenticator during the same ceremony.

## OATH seed enrollment

OATH provisioning is currently disabled. When accepted:

1. Prefer the service's add-authenticator flow while an independent login/recovery path is active.
2. Import the seed through the approved authenticated/on-device method.
3. Confirm issuer/account/algorithm/digits/period or counter on Pepper before storage.
4. Verify one code against the service without capturing it.
5. Reboot and verify again.
6. Delete any transient host representation according to the approved provisioning design.

Do not use a plaintext microSD import file as a production shortcut.

## Challenge slots

Challenge provisioning is currently disabled. When accepted, create keys on-device, label slots without account secrets, bind allowed operations, test with synthetic challenges, and rotate verifier state before deleting/replacing a slot.

## PIV/OpenPGP

Smart-card provisioning is currently disabled. When accepted, decide on-device generation versus reviewed import per key role. Keep administrative PIN/management material out of shell history and screenshots. Verify public keys/certificates and role-specific operations before relying on the device.

## Recovery export/import

Portable recovery is currently disabled. When accepted, use the dedicated ceremony in `docs/protocols/RECOVERY.md`. Do not assume app-data copy, cloud sync, or a dashboard backup is portable recovery.

## Abort conditions

Stop immediately for:

- unexpected device/firmware/artifact identity,
- missing independent authenticator or recovery codes,
- active competing USB/RPC owner,
- storage, battery, temperature, or resource-gate concern,
- request cue mismatch,
- unexpected state creation/reset/migration,
- write/read-back mismatch,
- capability not marked ready for the exact operation,
- any request to expose or log secret material.
