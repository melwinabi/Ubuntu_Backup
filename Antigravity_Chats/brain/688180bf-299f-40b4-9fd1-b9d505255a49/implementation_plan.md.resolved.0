# Bug Correction and Project Cleanup Plan

This plan addresses several bugs identified during analysis, including typos, UI inconsistencies, and missing routes.

## Proposed Changes

### Core UI Improvements
---
#### [MODIFY] [layout.tsx](file:///home/melwinabi/Documents/Smart_Hub/src/app/layout.tsx)
- Correct typo "Smaer-HUB" to "Smart-HUB" in metadata title.

#### [MODIFY] [page.tsx](file:///home/melwinabi/Documents/Smart_Hub/src/app/page.tsx)
- Correct typo "Smaer-HUB" to "Smart-HUB" in the main header.

#### [MODIFY] [admin/dashboard/page.tsx](file:///home/melwinabi/Documents/Smart_Hub/src/app/admin/dashboard/page.tsx)
- Correct typo "Smaer-HUB" to "Smart-HUB" in the sidebar.

### Dashboard Functionality
---
#### [MODIFY] [dashboard/page.tsx](file:///home/melwinabi/Documents/Smart_Hub/src/app/dashboard/page.tsx)
- Update connection status indicator:
    - Display "Select a Device" or "Offline" when no device is selected (empty topics).
    - Ensure `signOut` consistently redirects to `/`.
- Improve logout reliability.

### Routing Fixes
---
#### [NEW] [admin/page.tsx](file:///home/melwinabi/Documents/Smart_Hub/src/app/admin/page.tsx)
- Simple component that redirects to `/admin/dashboard` to fix the 404 on `/admin`.

#### [NEW] [login/page.tsx](file:///home/melwinabi/Documents/Smart_Hub/src/app/login/page.tsx)
- Simple component that redirects to `/` to fix the 404 on `/login`.

### Data Integrity (Optional/TBD)
---
- Seeding or cleanup of test data if requested, or just ensuring the UI handles empty states gracefully.

## Verification Plan

### Automated Verification
- Run `npm run lint` and `npx tsc --noEmit` to ensure no regressions.

### Browser-based Verification (via Browser Subagent)
1.  **Check Landing Page**: Verify title and header say "Smart-HUB".
2.  **Check Dashboard**: 
    - Verify status shows "Connecting..." only when actually connecting to MQTT.
    - Verify "Add Device" modal reflects available devices (I may need to create a test device).
    - Verify Logout redirects to `/`.
3.  **Check Admin Dashboard**: Verify sidebar title and access to `/admin/dashboard`.
4.  **Verify Redirects**: Visit `/admin` and `/login` and ensure they redirect correctly.
