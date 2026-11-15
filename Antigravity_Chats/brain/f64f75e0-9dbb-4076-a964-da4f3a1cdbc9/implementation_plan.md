# Discord Update Procedure for Linux

Discord on Linux often requires manual updates when a new version is released. This plan outlines the step-by-step process used to update Discord from a `.deb` package, which is the file found in your Downloads folder.

## Proposed Changes

We will not be changing any code, but rather performing a system-level update using the terminal.

### Update Steps

1.  **Locate the Update File**: We have identified the latest download in your Downloads folder: `discord-0.0.131.deb`.
2.  **Verify Current Installation**: We have confirmed that Discord is currently installed at `/usr/share/discord` and its version is `0.0.131`.
3.  **Install the Update**:
    - Open the terminal.
    - Navigate to the Downloads folder.
    - Run the command: `sudo apt install ./discord-0.0.131.deb`
    - Enter your password to authorize the installation.
4.  **Relaunch Discord**: Close any running instances of Discord and relaunch it from your application menu.

## Verification Plan

### Automated Tests
- Run `discord --version` after installation to ensure it reports the correct version.
- Run `which discord` to verify the execution path.

### Manual Verification
- Launch Discord and check if the "Update Available" screen still appears.
- Ensure the application opens and connects successfully.
