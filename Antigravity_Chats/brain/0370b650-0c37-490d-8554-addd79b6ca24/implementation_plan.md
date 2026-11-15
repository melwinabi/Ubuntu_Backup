# Update Smart Hub Implementation Plan

This plan outlines the steps to implement an advanced device configuration system where admins can add devices, select widgets, and store technical configurations (MQTT/Server ID).

## Proposed Changes

### 1. Database Schema Update
- Modify `prisma/schema.prisma` to include:
    - `series`: String (e.g., "W1")
    - `mqttTopic`: String
    - `serverId`: String
    - `widgets`: Json (to store array of widget configurations)
- Run `prisma generate` and `prisma migrate dev`.

### 2. Admin Dashboard Enhancements
- **"Add Device" Flow**:
    - Add an "Add Device" button to the management section.
    - Implement a step-by-step form:
        1. **Series Selection**: Select "W1 Series".
        2. **Product Type**: Select "Switch" or "Smart Hub".
        3. **Widget Selection**:
            - If **Switch**: Default to one "Switch" widget.
            - If **Smart Hub**: Allow selecting multiple from: Temperature, Humidity, Pressure, Light, Rain, Switch, Presence, Read Value, Display, Push Button, Slider.
        4. **Technical Config**: Enter MQTT Topic, Device ID (numerical), and Server ID.
- **Storage**: Update API to save these settings.

### 3. User Window/Dashboard
- Update the device search result to dynamically render widgets based on the `widgets` JSON field in the database.
- Each widget type will have a corresponding UI component (e.g., a gauge for Temp/Hum, a toggle for Switch).

### 4. Brand Consistency
- Rename "Smaer-HUB" to "Smart-HUB" globally.

## Verification Plan

### Manual Verification
1. **Admin**: Add a "Smart Hub" device with "Temperature" and "Switch" widgets.
2. **Admin**: Enter MQTT topic "house/livingroom/hub1".
3. **User**: Search for the new Device ID.
4. **User**: Verify that both Temperature (display) and Switch (toggle) widgets appear.
5. **User**: Verify that selecting "Switch" (product type) only shows the Switch widget.
