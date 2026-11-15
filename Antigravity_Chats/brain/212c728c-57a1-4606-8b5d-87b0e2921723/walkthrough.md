# Walkthrough - DbGate Reinstallation and Fix

We have successfully addressed the issue where DbGate was not opening due to sandbox restrictions on Ubuntu 24.04. We switched to the community AppImage version to ensure it works reliably without complex root-level permission changes.

## What Was Done

### 1. Switched to AppImage
- Downloaded the latest community version of DbGate (v7.1.8) as an AppImage to `~/Downloads/dbgate.AppImage`.
- Extracted the AppImage to `~/DbGate` to avoid the `libfuse2` dependency issue which is common on newer Ubuntu versions.

### 2. Launch Fix (Ubuntu 24.04)
- Fixed a bug in the DbGate `AppRun` script that prevented it from launching when command-line arguments (like `--no-sandbox`) were passed.
- Configured the application to run with the `--no-sandbox` flag, which is required for Electron apps on Ubuntu 24.04 when unprivileged user namespaces are restricted.

### 3. Desktop Integration
- Created a new desktop shortcut: **DbGate**.
- You can now find it in your application menu and launch it directly.

## Verification Summary

- **Manual Launch**: Verified that `~/DbGate/AppRun --no-sandbox` starts the application without FATAL sandbox errors.
- **Shortcut Verification**: Verified the desktop entry with `gtk-launch`.
- **System Integrity**: The new version is isolated in `~/DbGate`, ensuring it doesn't conflict with any system packages.

## Note on Cleanup
The old version (v7.1.6) may still be installed on your system. If you haven't already, you can remove it by running:
`sudo apt-get remove --purge dbgate -y`
