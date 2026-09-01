# Application Submission

- PepperKey is a hardened USB U2F and CTAP1 authenticator derived from the official Flipper Zero U2F application. It starts locked, requires long-hold arming and approval, binds approval to the complete application identifier, rejects invalid handles before prompting, fails closed on invalid state, and locks after one successful operation.

# Extra Requirements

- Requires a Flipper Zero, a USB data cable, and a browser or relying party that still accepts U2F and CTAP1 authenticators. It does not support FIDO2, CTAP2, resident passkeys, NFC, or Bluetooth.

# Author Checklist (Fill this out)

- [ ] I've read the contribution guidelines and my PR follows them
- [ ] I own the code I'm submitting or have code owner's permission to submit it
- [ ] I have performed a self-review of my own code
- [ ] I have commented my code, particularly in hard-to-understand areas
- [ ] I have validated the manifest with the official bundle command
- [ ] The referenced AdaTech source commit matches the reviewed and device-tested build
- [ ] The screenshot came from the accepted build and contains no account or device identifiers

# AI usage disclosure (Fill this out)

- Partially AI assisted. PepperKey is derived from Flipper Devices' GPLv3 U2F implementation. Codex produced the PepperKey hardening changes, documentation, catalog metadata, and static checks under user direction. The source identifies the pinned upstream release and describes the material changes.

# Reviewer Checklist (Don't fill this out, and don't remove it from the template)

- [ ] Bundle is valid
- [ ] There are no obvious issues with the source code
- [ ] I've ran this application and verified its functionality
