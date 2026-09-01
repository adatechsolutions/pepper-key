# PepperKey

PepperKey turns a Flipper Zero into a deliberately gated USB U2F authenticator. It is derived from the official Flipper Zero U2F application and keeps a separate app identity and credential store.

## Safety controls

- Starts locked and requires a long OK hold to arm a two-minute session.
- Requires a long OK hold for every registration and sign-in.
- Shows a short fingerprint of the requesting U2F application identifier.
- Binds approval to the complete application identifier captured for the prompt.
- Rejects unknown handles and check-only probes before showing approval.
- Locks after one successful registration or sign-in.
- Fails closed instead of replacing an unreadable identity or resetting its counter.
- Ends USB mode and clears the active session after terminal data errors.
- Keeps its data separate from the stock U2F application.

## Important limits

- PepperKey supports legacy U2F and CTAP1 only.
- It does not support FIDO2, CTAP2, resident passkeys, NFC, or Bluetooth.
- It is not FIDO certified and is not a replacement for a purpose-built certified authenticator.
- Use it only as a secondary authenticator after saving recovery codes and registering an independent backup authenticator.
