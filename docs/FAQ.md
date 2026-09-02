# PepperKey FAQ

## Is PepperKey an all-in-one authenticator today?

It is an all-in-one product architecture under active development. The U2F path is device-tested with remaining release gates. HOTP/TOTP and challenge-response cores pass native tests. CTAP2/passkeys, device OTP storage/UI, smart card, wireless authentication, and portable recovery remain disabled or research-only.

## Why not turn every capability on immediately?

Authenticator discovery flags route real credential requests into the device. Exposing a half-implemented protocol can lose credentials or weaken approval/storage behavior. PepperKey therefore keeps advertisement false until the whole capability passes.

## Is it a YubiKey replacement?

No. Flipper Zero is general-purpose, user-flashable hardware. PepperKey does not claim secure-element tamper resistance, biometric verification, FIDO certification, managed enterprise provisioning, or equivalent assurance to a purpose-built certified token.

## Does the all-zero AAGUID mean the current U2F key is wrong?

No. The tested path is CTAP1/U2F, which does not provide a CTAP2 AAGUID. PepperKey's future CTAP2 model AAGUID is `34e4a441-9a60-4361-830a-cfa20bff3147` and remains unadvertised.

## Can I store TOTP codes now?

No live OTP seed should be provisioned yet. The algorithm core passes RFC vectors, but encrypted device storage, provisioning, RTC/drift UI, atomic HOTP counters, deletion, and recovery are still gated.

## Can PepperKey hold passkeys?

Not yet. CTAP2 transport, CBOR, credentials, PIN/UV, management, extensions, conformance, browser testing, and storage fault handling must land together before passkeys are advertised.

## Can it sign SSH or GPG operations?

SSH FIDO signing depends on the future accepted CTAP2 implementation. OpenPGP-card support needs a separate USB CCID/firmware track. Neither is currently available.

## Can it generate Windows smart-card or PIV credentials?

PIV is a research track. It requires USB CCID, APDU processing, PIN/retry policy, certificate/key provisioning, and interoperability testing.

## Can credentials move to another Flipper?

Current U2F state is device-bound. Same-device recovery restore is still an open acceptance gate, and cross-device portable recovery is disabled because it creates cloning and rollback risks.

## What does core-ready mean?

A bounded portable component passes named vectors or policy tests. It does not mean there is a device UI, secure persistent store, host protocol, compatibility result, release artifact, or support promise.

## What should I use PepperKey for now?

Only controlled secondary-authenticator testing with another working authenticator and offline recovery codes. Do not use it as the sole login or recovery method for an important account.
