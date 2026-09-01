# Build

## Pinned inputs

- Flipper target: F7
- Firmware and SDK: 1.4.3 / API 87.1
- Firmware commit: `8622f1a2b83d8f4918dd5fa3f43de963f6d6f819`
- uFBT: 0.2.6

The installed uFBT SDK must remain pinned to 1.4.3 for the version 0.1 acceptance build. Do not update the SDK in the middle of a release build.

## Guarded release command

From the repository root on the authorized AdaTech build host:

```sh
./scripts/build-release.sh
```

The script uses AdaTech's configured resource and heavy-job helpers, verifies uFBT 0.2.6 / target F7 / SDK 1.4.3, runs both source-policy suites and the SDK linter, and then builds. Tool paths can be supplied through `CODEX_RESOURCE_CHECK_BIN`, `CODEX_HEAVY_RUN_BIN`, and `UFBT_BIN`. It does not launch, upload, install, or flash anything.

CI uses the official 1.4.3 F7 SDK archive pinned by SHA-256. A local direct `ufbt faps` command is for development only and is not release evidence.

The standard uFBT FAP includes a four-byte `.gnu_debuglink` checksum for the host-specific debug ELF. Linux CI and macOS release builds can therefore have different whole-file SHA-256 values even when their runtime sections and assets are identical. The release receipt must identify and distribute the exact FAP used for physical acceptance; do not substitute a different host build by comparing source alone.

Expected build artifact:

```text
app/dist/pepper_key.fap
```

Installation and device testing require the independent gates in `ACCEPTANCE.md`.
