## Change

Describe the behavior and why it is needed.

## Security boundary

Describe any effect on arming, approval binding, storage, counter persistence, USB lifecycle, secret handling, or release provenance.

## Verification

- [ ] `sh tests/check-source.sh`
- [ ] `sh tests/check-security-fixes.sh`
- [ ] `(cd app && ufbt lint)`
- [ ] Pinned FAP build, if applicable
- [ ] Device/browser checks listed with untested paths called out
- [ ] No secrets, app data, identifiers, or authentication captures included
- [ ] User-facing claims remain within `docs/marketing/CLAIMS-GUIDE.md`
