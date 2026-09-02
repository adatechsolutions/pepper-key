# Standards and authoritative references

This index records protocol targets; it does not claim conformance or certification.

## FIDO and WebAuthn

- FIDO CTAP 2.2 Proposed Standard, dated 2025-07-14: <https://fidoalliance.org/specs/fido-v2.2-ps-20250714/fido-client-to-authenticator-protocol-v2.2-ps-20250714.html>
- W3C Web Authentication Level 3: <https://www.w3.org/TR/webauthn-3/>
- FIDO U2F Raw Message Formats: <https://fidoalliance.org/specs/fido-u2f-v1.2-ps-20170411/fido-u2f-raw-message-formats-v1.2-ps-20170411.html>

## OTP and encoding

- RFC 4226, HOTP: <https://www.rfc-editor.org/rfc/rfc4226.html>
- RFC 6238, TOTP: <https://www.rfc-editor.org/rfc/rfc6238.html>
- RFC 4648, Base-N encodings: <https://www.rfc-editor.org/rfc/rfc4648.html>

## Smart card

- NIST SP 800-73-5 PIV Part 2: <https://csrc.nist.gov/pubs/sp/800/73/pt2/5/final>
- GnuPG OpenPGP card specifications: <https://gnupg.org/ftp/specs/>
- USB-IF CCID class documents: <https://www.usb.org/document-library/device-class-specification-usb-chipcard-interface-devices-110>

## Flipper platform

- External FAP documentation: <https://developer.flipper.net/flipperzero/doxygen/apps_on_sd_card.html>
- Flipper Zero hardware specifications: <https://docs.flipper.net/zero/development/hardware/tech-specs>
- U2F HID API header: <https://developer.flipper.net/flipperzero/doxygen/furi__hal__usb__hid__u2f_8h_source.html>

## Versioning policy

Before moving a capability to `accepted`:

1. Confirm the exact normative specification version and publication state.
2. Record required/optional/deprecated behaviors and the PepperKey subset.
3. Add traceable conformance tests and negative cases.
4. Review privacy, attestation, algorithm, transport, and downgrade effects.
5. Update this file, the protocol document, release checklist, compatibility matrix, and claims guide.

A newer draft or library release is not adopted automatically. It is a reviewed product and security change.
