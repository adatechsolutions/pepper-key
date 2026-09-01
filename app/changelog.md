# Changelog

## v0.1

- Added explicit long-hold arming and approval.
- Added a two-minute session limit and one-success lock.
- Bound approvals to the complete U2F application identifier.
- Added request length checks, constant-time handle comparison, and secret zeroization.
- Persisted the authentication counter before returning a signed response.
- Made identity provisioning fail closed on corrupt, incomplete, or unreadable state.
- Limited new encrypted records to the exact 32-byte ciphertext output.
- Added terminal-error USB teardown with a non-blocking worker-to-UI event bridge.
- Isolated app data and packaged assets from the stock U2F application.
