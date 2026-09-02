#!/usr/bin/env python3
import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
document = json.loads((root / "capabilities.json").read_text(encoding="utf-8"))

assert document["schema_version"] == 1
assert document["product"] == "PepperKey"
allowed = {"accepted", "device-tested", "core-ready", "implementation", "research", "blocked"}
required = {
    "u2f",
    "ctap2",
    "passkeys",
    "oath-totp",
    "oath-hotp",
    "challenge-response",
    "ssh-fido",
    "piv",
    "openpgp-card",
    "ble-transport",
    "nfc-transport",
    "recovery",
}

seen = set()
for capability in document["capabilities"]:
    identifier = capability["id"]
    assert identifier not in seen, f"duplicate capability: {identifier}"
    seen.add(identifier)
    assert capability["status"] in allowed, f"invalid status: {identifier}"
    assert capability["security_domain"]
    assert capability["release_gate"]
    if capability["status"] in {"accepted", "device-tested"}:
        assert capability["evidence"], f"tested capability lacks evidence: {identifier}"

missing = required - seen
assert not missing, f"missing required capabilities: {sorted(missing)}"
assert next(c for c in document["capabilities"] if c["id"] == "u2f")["status"] == "device-tested"
assert next(c for c in document["capabilities"] if c["id"] == "ctap2")["status"] != "accepted"

readme = (root / "README.md").read_text(encoding="utf-8")
security = (root / "SECURITY.md").read_text(encoding="utf-8")
for identifier in required:
    assert f"`{identifier}`" in readme or f"`{identifier}`" in security, (
        f"capability is absent from public docs: {identifier}"
    )

print("PepperKey capability policy checks passed.")
