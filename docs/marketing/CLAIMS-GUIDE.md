# PepperKey claims guide

This guide is mandatory for repository copy, the Apps Catalog, AdaTech pages, demos, ads, social posts, partner outreach, and support replies.

## Alpha suite rule

The Apps Catalog and v0.1 release remain U2F-only. The 0.2 alpha repository may say:

- “Modular authenticator foundation in development.”
- “Portable HOTP/TOTP core passes published RFC vectors.”
- “Challenge-response and isolated vault-domain foundations are implemented in source.”
- “CTAP2/passkeys, device OTP vault/UI, PIV, OpenPGP, Bluetooth/NFC authentication, and portable recovery remain gated.”

`core-ready` must never be shortened to “supported,” “works,” “device-ready,” or “released.” A roadmap item must never be shown as an active device capability.

## Approved factual claims

- “USB U2F/CTAP1 application for Flipper Zero.”
- “Starts locked and requires a long local hold to arm.”
- “Requires a long local hold for each registration or sign-in approval.”
- “Locks after one successful operation or after the session timeout.”
- “Keeps PepperKey app data separate from the stock U2F app.”
- “Fails closed on incomplete, malformed, unreadable, or undecryptable identity state.”
- “Open-source under GNU GPL version 3.”
- “Maintained by Ada Tech Solutions Inc.”
- “For secondary-authenticator use after testing with an independent recovery path.”

Only attach a platform/browser/service compatibility claim to a dated acceptance receipt for that exact combination.

## Required qualifiers

- Legacy U2F/CTAP1 over a directly connected USB cable only.
- No PepperKey capability is FIDO Certified.
- Not FIDO2/CTAP2, a passkey provider, NFC, or Bluetooth/BLE in v0.1.
- Flipper Zero is general-purpose, user-flashable hardware without PepperKey-specific tamper resistance.
- Browser and relying-party support varies.
- Use an independent authenticator and recovery codes; do not make PepperKey the sole authenticator for an important account.

## Prohibited claims

- “Unhackable,” “military-grade,” “bank-grade,” “enterprise-grade,” “certified,” or “guaranteed secure.”
- “Phishing-proof,” “passkey-ready,” “FIDO2-ready,” “CTAP2-ready,” “Bluetooth security key,” “all features enabled,” or “works everywhere” before the named capability is accepted.
- “Replacement” or “equivalent” language for a named commercial security-key brand.
- Any FIDO certification mark, Flipper logo, or wording that implies endorsement, partnership, or affiliation.
- “Audited” without the qualifier “independent multi-pass static review”; do not imply a paid third-party audit.
- Statements that the short screen fingerprint proves a human-readable site identity or transaction details.

## Evidence labels

- **Built:** source compiled for a pinned target/API.
- **Device-tested:** exact FAP installed and exercised on the named device/firmware.
- **Compatible:** disposable-account registration and authentication passed for the named platform/browser/service.
- **Catalog-listed:** accepted by Flipper Apps Catalog moderation.
- **Certified:** never use unless an applicable certification body grants it in writing.
- **Core-ready:** bounded portable code passes named vectors; no on-device availability implied.
- **Research:** architecture/dependency investigation only.
