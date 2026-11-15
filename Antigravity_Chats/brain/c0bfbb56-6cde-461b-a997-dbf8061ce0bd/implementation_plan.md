# Implementation Plan: Device & Widget Management

This plan outlines the steps to implement the "Add Device" workflow for Admins and the dynamic "Widget Display" for Users.

## Database Changes
We will update `prisma/schema.prisma` to incorporate the relational structure requested:
- `DeviceSeries`: Stores series names (e.g., W1_Series).
- `DeviceSubSeries`: Stores sub-series linked to a series (e.g., W1_Smart_Hub, W1_Switch).
- `Device`: Updated to link to `DeviceSeries` and `DeviceSubSeries`.
- `Widget`: Stores individual widgets for a device with their MQTT topics.

## API Endpoints
- `GET /api/series`: List all series.
- `GET /api/sub-series/[seriesId]`: List sub-series for a specific series.
- `POST /api/devices`: Unified endpoint to save a device and its widgets.
- `GET /api/devices/user/[seriesId]`: List device IDs for a series.
- `GET /api/devices/[deviceId]/widgets`: Fetch all widgets for a device.

## Admin Features
- Add an "Add Device" button to the Admin Dashboard.
- Modal-based multi-step workflow:
  1. Series selection.
  2. Sub-series selection.
  3. Dynamic input fields based on sub-series selection.
  4. Widget selection (checkbox list).
  5. Widget-topic mapping.
  6. Final Save.

## User Features
- Update User Dashboard to include Series and Device ID selection.
- "Display Widgets" button to trigger dynamic loading.
- Reusable `Widget` component system with specialized rendering:
  - `NumericWidget`: Temperature, Gas, Voltage, Current.
  - `PercentageWidget`: Humidity.
  - `ToggleWidget`: Light, Relay.
  - `ButtonWidget`: Push Button.
  - `IndicatorWidget`: Motion Sensor.

## Verification
- Manual test of device creation via Admin panel.
- Verification of database records (PostgreSQL).
- UI check for widget rendering in User panel.
