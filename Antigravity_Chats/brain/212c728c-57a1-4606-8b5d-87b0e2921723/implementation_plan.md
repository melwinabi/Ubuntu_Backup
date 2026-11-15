# Clean Uninstall and AppImage Setup

The user wants a full uninstallation of the current version and a working "normal" version. We will switch to the AppImage version to avoid complex installation issues on Ubuntu 24.04.

## User Review Required

> [!IMPORTANT]
> - You will need to run one command in your terminal to uninstall the current package.
> - We will use the AppImage version of DbGate, which doesn't require "installation" and is easy to fix for the Ubuntu 24.04 sandbox issue.

## Proposed Changes

### 1. Clean Uninstall
- Run `sudo apt-get remove --purge dbgate -y` to remove the existing `.deb` installation.

### 2. AppImage Setup
- Download the latest `DbGate-7.1.8.AppImage`.
- Make it executable.
- Create a launch script/shortcut that uses the `--no-sandbox` flag.

## Verification Plan

### Manual Verification
1. Run the AppImage from the terminal with `--no-sandbox` and verify it opens.
2. Verify the community version features are available (normal version).
