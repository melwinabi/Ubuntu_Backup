# Implementation Plan - Persistent User Dashboards

Currently, users have to search for their devices every time they log in. This plan implements a persistent dashboard where devices added by a user are stored in the database and automatically fetched upon login.

## User Review Required

> [!IMPORTANT]
> This change involves a database schema update. I will run `npx prisma db push` to apply these changes. This will add two new tables: `users` and `user_devices`. Existing device data will not be affected.

## Proposed Changes

### [Database]

#### [MODIFY] [schema.prisma](file:///home/melwinabi/Documents/Smart_Hub/prisma/schema.prisma)
-   Add `User` model with `email` (unique).
-   Add `UserDevice` model to link `User` and `Device`.

### [API]

#### [NEW] [route.ts](file:///home/melwinabi/Documents/Smart_Hub/src/app/api/user/devices/route.ts)
-   `GET`: Fetch all devices associated with the logged-in user's email.
-   `POST`: Link a `deviceId` to the current user's email.

### [Frontend]

#### [MODIFY] [page.tsx](file:///home/melwinabi/Documents/Smart_Hub/src/app/dashboard/page.tsx)
-   **On Mount**: Fetch user devices from `/api/user/devices`.
-   **State Logic**:
    -   If devices exist, set the first one as active and show widgets.
    -   If no devices, show the "Search Your Device" screen.
-   **Add Flow**: When a device is selected and "Display Widgets" is clicked, if it's not already saved, call the API to save it to the user's account.
-   **Navigation**: Add a "Manage Devices" or "Add New Device" button to return to the search screen from the widget view.

## Verification Plan

### Automated Tests
-   Verify `/api/user/devices` returns empty for a new user.
-   Verify `/api/user/devices` correctly stores and returns a device after it's added.

### Manual Verification
-   Log in as a user (`asmelwin3@gmail.com`).
-   Search for a device and display widgets.
-   Refresh the page.
-   **Expected**: The dashboard should immediately show the same device without searching again.
