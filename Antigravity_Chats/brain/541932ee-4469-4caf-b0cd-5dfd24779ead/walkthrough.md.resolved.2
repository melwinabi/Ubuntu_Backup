# Walkthrough - OneDrive Setup Complete

The OneDrive setup is now finished. The `onedrive` client (abraunegg/onedrive) has been installed, authenticated, and configured to run automatically in the background.

## Changes Made

### 1. Repository Configuration
- Added the official OpenSuSE Build Service (OBS) repository release key for Ubuntu 24.04.
- Configured the OBS repository for supported `onedrive` client packages.

### 2. Client Installation
- Installed the latest `onedrive` client via `apt`.
- Verified the version and connectivity to Microsoft services.

### 3. Authentication
- Authenticated the client using a Microsoft account redirect URI provide by the user.

### 4. Background Sync
- Enabled and started the `systemd` user service to handle automatic file synchronization.
- Verified that the service is running and active using `systemctl` and `journalctl`.

## Verification Results

### Service Status
The OneDrive background service is active and sincronizing files.

```bash
systemctl --user status onedrive
```
Output snippets:
- `Active: active (running)`
- `Starting a sync with Microsoft OneDrive`
- `Fetching items from the OneDrive API...`

### Sync Directory
By default, your files are now synced to:
`~/Documents/OneDrive`

## How to Manage OneDrive

### Adding Files and Folders
Any file or folder you **move or copy** into `~/Documents/OneDrive` will:
1. Be automatically detected by the sync service.
2. Be **uploaded** to your Microsoft OneDrive cloud storage.
3. Become **available** on the OneDrive website and any other devices (phones, tablets, other PCs) connected to the same account.

> [!NOTE]
> If you **delete** a file from this folder, it will also be deleted from your OneDrive cloud (and vice versa).

### Core Commands
- **Check logs**: `journalctl --user -u onedrive -f`
- **Stop sync**: `systemctl --user stop onedrive`
- **Start sync**: `systemctl --user start onedrive`
