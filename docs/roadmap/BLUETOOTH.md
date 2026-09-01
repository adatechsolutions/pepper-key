# Bluetooth roadmap boundary

PepperKey v0.1 is USB U2F/CTAP1 only. Bluetooth/BLE is not hidden, disabled, or awaiting a marketing switch; it is absent by design.

A future Bluetooth experiment must be treated as a separate security and product track because transport pairing, session binding, discoverability, replay resistance, host compatibility, background lifecycle, user-presence signaling, and Flipper firmware interfaces differ materially from the directly connected USB model.

Before any implementation or public claim, the track needs:

- A supported protocol decision; “U2F over BLE” must not be invented as a proprietary transport.
- Current official Flipper BLE API and Apps Catalog policy verification.
- Threat model and pairing/re-pairing UX.
- Explicit separation from PepperKey USB identity unless safe migration is proven.
- New parser, lifecycle, disconnect, timeout, queue-pressure, and recovery tests.
- Platform/browser compatibility evidence for every claimed target.
- Independent review and its own versioned release receipt.

Do not add Bluetooth wording to v0.1 ads, screenshots, Catalog metadata, app name, or compatibility claims. Research may proceed in a separate branch after USB v0.1 is accepted; it should not create a second public repository unless the architecture and governance genuinely diverge.
