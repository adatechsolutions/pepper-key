# PepperKey v0.1 product specification

## Ownership and release line

- Product owner and publisher: Ada Tech Solutions Inc.
- Canonical source: `https://github.com/adatechsolutions/pepper-key`
- License: GNU GPL version 3.
- Hardware: Flipper Zero target F7.
- Pinned official firmware/SDK: 1.4.3 / API 87.1.
- Transport and protocol: directly connected USB U2F/CTAP1 only.

## Intended use

PepperKey is a secondary, owner-present authenticator for compatible legacy U2F registration and authentication. It is intended for enthusiasts, controlled labs, training, development, and carefully tested secondary-account use.

It is not a sole authenticator, recovery device, certified authenticator, passkey provider, CTAP2 device, or enterprise identity product.

## Security state machine

1. **Locked:** no PepperKey USB HID interface and no mutable identity decrypted.
2. **Arming:** a long local OK hold initializes or loads state. New identity provisioning occurs only when both records are conclusively absent.
3. **Armed:** USB U2F is exposed for at most two minutes.
4. **Pending:** a valid registration or authentication request displays a short application-hash fingerprint and waits up to ten seconds.
5. **Approved:** a long local OK hold authorizes one request bound to the complete application hash and request type.
6. **Locking:** one successful operation displays success and locks after three seconds.
7. **Terminal error:** all timers and HID stop, the prior USB mode is restored, secrets are zeroized/freed, and the UI remains locked with a retry instruction.

## Persistent data

- `/ext/apps_data/pepper_key/key.u2f`: device-bound encrypted credential-derivation root.
- `/ext/apps_data/pepper_key/cnt.u2f`: device-bound encrypted global authentication counter.

The records are not portable to another Flipper. Deleting both creates a new identity on the next explicit arm. Missing one record, malformed state, I/O failure, or decryption failure must never trigger automatic replacement or counter reset.

## Release requirements

- Exact reviewed source and immutable AdaTech commit.
- Pinned uFBT/SDK build and FAP SHA-256.
- Independent security review with confirmed findings remediated.
- Physical-device state, timeout, fault, USB restoration, and recovery checks.
- Disposable-account registration and authentication on each claimed platform.
- Genuine screenshot from the accepted build.
- Catalog bundle validation and moderation.

Claims and support promises may not exceed the evidence in the current release receipt.
