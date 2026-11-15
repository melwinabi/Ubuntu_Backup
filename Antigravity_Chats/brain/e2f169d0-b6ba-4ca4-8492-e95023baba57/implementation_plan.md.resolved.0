# Implementation Plan - Smaer-HUB Home Automation Management System

This plan outlines the development of the Smaer-HUB, a secure web-based system for managing IoT devices in the W1 series.

## User Review Required

> [!IMPORTANT]
> The application will require real Google OAuth credentials (`GOOGLE_CLIENT_ID`, `GOOGLE_CLIENT_SECRET`) and an SMTP server for OTP emails. These must be provided in an `.env.local` file.

> [!WARNING]
> This plan assumes a running PostgreSQL instance accessible via a `DATABASE_URL`.

## Proposed Changes

---

### Project Initialization

#### [NEW] [next.config.js](file:///home/melwinabi/.gemini/antigravity/scratch/smart-hub/next.config.js)
Configuration for Next.js, including environment variables and potential image domain allows for Google profiles.

---

### Database Layer

#### [NEW] [schema.sql](file:///home/melwinabi/.gemini/antigravity/scratch/smart-hub/prisma/schema.prisma)
We will use Prisma ORM for interacting with PostgreSQL. The schema will include:
- `admin_emails` table: `id`, `email`.
- `devices` table: `id`, `device_name`, `product_type`, `status`, `last_seen`.
- `otp_verifications` table: `id`, `email`, `otp`, `expires_at`.

---

### Authentication Layer

#### [NEW] [auth.ts](file:///home/melwinabi/.gemini/antigravity/scratch/smart-hub/lib/auth.ts)
Implementation of NextAuth with:
- `GoogleProvider` for users.
- Custom `CredentialsProvider` for admin OTP flow.

#### [NEW] [api/auth/otp/route.ts](file:///home/melwinabi/.gemini/antigravity/scratch/smart-hub/app/api/auth/otp/route.ts)
Endpoint to:
1. Verify if the admin email exists in `admin_emails`.
2. Generate and store a 6-digit OTP.
3. Send OTP via SMTP (Nodemailer).

---

### Dashboard & Device Management

#### [NEW] [api/devices/route.ts](file:///home/melwinabi/.gemini/antigravity/scratch/smart-hub/app/api/devices/route.ts)
REST API to search for devices by `id`.

#### [NEW] [page.tsx](file:///home/melwinabi/.gemini/antigravity/scratch/smart-hub/app/page.tsx)
The Login Page with User (Google) and Admin (Email/OTP) options.

#### [NEW] [admin/dashboard/page.tsx](file:///home/melwinabi/.gemini/antigravity/scratch/smart-hub/app/admin/dashboard/page.tsx)
Admin dashboard featuring:
- Search input for Device ID.
- Selection for "W1 Smart Hub" and "W1 Switch".
- Results display for device information.

---

### UI Components & Styling

#### [NEW] [globals.css](file:///home/melwinabi/.gemini/antigravity/scratch/smart-hub/app/globals.css)
Implementation of a modern, premium design system:
- Dark mode by default or system preference.
- Glassmorphism effects for cards and dashboards.
- Hover transitions and loading spinners.

---

## Verification Plan

### Automated Tests
- Unit tests for OTP generation and expiration logic.
- Integration tests for API endpoints (`/api/devices`).

### Manual Verification
1. **Login Flow**:
   - Verify Google OAuth redirects and user session creation.
   - Test Admin login with correct and incorrect email.
   - Verify OTP receipt and validation (within 5 min window).
2. **Dashboard functionality**:
   - Search for Device ID 1 and 2.
   - Verify "Device not found" message for non-existent IDs.
3. **UI/UX**:
   - Check responsiveness on mobile and tablet views.
   - Verify loading states during search.
