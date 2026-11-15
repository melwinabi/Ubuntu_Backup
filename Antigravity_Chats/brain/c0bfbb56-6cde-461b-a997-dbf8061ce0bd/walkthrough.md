# Walkthrough: Device & Widget Management

I have extended the **Smaer-HUB Home Automation System** with a complete device creation and widget configuration workflow.

## New Features

### 1. Admin "Add Device" Workflow
Located in the Admin Dashboard, this multi-step process allows for precise configuration of IoT hardware:
- **Series & Sub-Series Selection**: Filter by series (e.g., W1_Series) and device types (e.g., Smart Hub, Switch).
- **Custom Configuration**: Dynamic fields for Device IDs, Server IDs, and Topics.
- **Widget Management**: For Smart Hubs, choose from 9 sensor/switch types and map each to specific MQTT topics.

### 2. Dynamic User Widgets
Users can now search for specific devices and view their active modules:
- **Device Search**: Guided selection through Series and available Device IDs.
- **Responsive Components**: Specialized UI for each widget type:
  - **Sensors**: Temperature, Humidity (Percentage), Gas, Motion.
  - **Controls**: Lights (Toggles), Relays (Switches), Push Buttons.
  - **Power**: Voltage and Current monitors.

## Technical Implementation

### Database
Transitioned to a relational schema for better scalability:
- [schema.prisma](file:///home/melwinabi/Documents/Smart_Hub/prisma/schema.prisma) now includes `DeviceSeries`, `DeviceSubSeries`, and `Widget` models.
- A [seed.ts](file:///home/melwinabi/Documents/Smart_Hub/prisma/seed.ts) script is available to populate the system with the initial W1 Series data.

### APIs
New RESTful endpoints handle the data flow:
- `GET /api/series` & `GET /api/sub-series`: Infrastructure discovery.
- `POST /api/devices`: Unified endpoint for atomic device and widget creation.
- `GET /api/devices`: Search and detailed inspection for both Admin and User panels.

## How to Verify
1.  **Configure Database**: Update `.env` with your `DATABASE_URL`.
2.  **Run Migration**: `npx prisma migrate dev`
3.  **Seed Data**: `npx ts-node prisma/seed.ts`
4.  **Admin Panel**: Navigate to `/admin/dashboard` and click **"Add Device"**. Follow the 5-step workflow.
5.  **User Panel**: Navigate to `/dashboard`, search for the newly created Device ID, and click **"Display Widgets"**.

---
Implementation completed. The system is now ready for production-level IoT management!
