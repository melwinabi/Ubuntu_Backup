# Project Analysis: Smaer-HUB Home Automation

This report summarizes the technical architecture and implementation details of the Smaer-HUB project.

## Core Infrastructure
- **Framework**: Next.js 16.2.2 (Breaking changes version).
- **React**: Version 19.2.4 (Uses React Canary/Stable features).
- **Database**: PostgreSQL with Prisma ORM.
- **Authentication**: NextAuth.js v4.

## UI & Design System
- **Style**: Premium Glassmorphism.
- **CSS**: Custom vanilla CSS in `globals.css` with CSS variables for the color palette.
- **Icons**: Lucide React.
- **Layout**: standard App Router layout with a custom `Providers` component wrapping the application in a `SessionProvider`.

## Authentication Flow
The application implements a dual-role authentication system:
- **Admin Login**: OTP-based authentication via `CredentialsProvider`. Includes a dummy fallback (`admin@demo.com` / `123456`) for testing.
- **User Login**: Google OAuth via `GoogleProvider`. Includes a `user-dummy` provider for quick demonstration.

## Key Components & Pages
- **Landing (Sign-in)**: `src/app/page.tsx` - Handles tabbed navigation between User and Admin logins.
- **Admin Dashboard**: `src/app/admin/dashboard/page.tsx` - Multi-step configuration flow for series management and device searching.
- **User Dashboard**: `src/app/dashboard/page.tsx` - Session-aware landing page for standard users.
- **API Routes**:
  - `/api/auth/[...nextauth]`: Auth configuration.
  - `/api/devices`: Device data retrieval (Prisma backed).

## Data Models (Prisma)
- `AdminEmail`: Stores authorized admin emails.
- `Device`: Main hardware entity with fields for name, type, series (defaulting to "W1"), and last seen status.
- `OtpVerification`: Temporary storage for OTP tokens.

---
Analysis completed. Ready for updates.
