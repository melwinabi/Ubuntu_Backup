# Smart Relay Switch App Implementation Plan

This plan outlines the architecture and design approach for developing a specialized Next.js (TypeScript) mobile-first app for controlling Smart Relay Switches.

## Goal Description
Develop a high-performance, mobile-first control interface for Smart Relay Switches. The app will feature a Neumorphic/Glassmorphic dark theme, offering a premium, tactile user experience. It will utilize MQTT for real-time, ultra-low latency hardware state synchronization, ensuring the UI accurately reflects the physical state of the switch to avoid "Ghost States".

## User Review Required
> [!IMPORTANT]
> The guidelines suggest using Vanilla CSS unless Tailwind CSS is explicitly requested. I will proceed with Vanilla CSS to craft the custom Neumorphic and Glassmorphic aesthetics. If you prefer Tailwind CSS, please let me know and specify the version you'd like to use.
>
> I will use `mqtt` (MQTT.js) to build a custom `MQTTProvider` context for Next.js, as it is standard and robust for handling real-time topics and events. 

## Open Questions
> [!WARNING]
> Do you have a specific MQTT broker URL (e.g., `ws://broker.hivemq.com:8000/mqtt` or a local one like `ws://localhost:9001`) that you'd like the app to connect to by default? 

## Proposed Changes

### Setup and Configuration
- Initialize a Next.js App Router project in the current directory with TypeScript and Vanilla CSS.
- Install dependencies: `lucide-react` for icons, `mqtt` for real-time communication.

### Core Application
#### [NEW] `src/context/MQTTProvider.tsx`
- A React Context provider that establishes the MQTT connection.
- Manages state for: `isConnected`, `switchState` (ON/OFF/LOADING), and an `events` array (last 5 MQTT messages).
- Exposes an `publishCmd(state: number)` function that publishes `CMD: 1` or `CMD: 0` to a command topic.
- Listens to the `STATUS` topic and updates the `switchState` only upon receiving the hardware ACK.

#### [NEW] `src/app/globals.css` / `src/app/page.module.css`
- Custom CSS defining the Neumorphic/Glassmorphic dark theme tokens (colors, box-shadows for the tactile switch effect, neon glow effects for the active state).

### UI Components
#### [NEW] `src/components/MasterToggle.tsx`
- The large, central tactile switch button.
- Transitions between OFF (subtle glow), LOADING (processing), and ON (vibrant neon green/blue outer glow) states based entirely on the MQTT context.

#### [NEW] `src/components/StatusMetadata.tsx`
- Displays Uptime, Signal Strength (Wi-Fi/LoRa icon from `lucide-react`), and Last Action text.

#### [NEW] `src/components/ControlFeatures.tsx`
- Contains the "Child Lock" toggle (disables hardware physical button).
- Contains the Timer/Schedule interface for "Auto-Off".

#### [NEW] `src/components/LoggingSection.tsx`
- A bottom scrollable pane displaying the last 5 raw MQTT events (e.g., `[20:15:02] Topic: switch/1/state -> ON`).

#### [MODIFY] `src/app/page.tsx` / `src/app/layout.tsx`
- Wrap the app with `MQTTProvider`.
- Assemble the components into a responsive, mobile-first layout.

## Verification Plan

### Automated Tests
- Validate code build using `npm run build`.

### Manual Verification
- Start the local development server with `npm run dev`.
- Preview the UI using the browser to ensure the Neumorphic aesthetic is premium and tactile.
- Use a local MQTT broker (like Mosquitto with WebSockets enabled) or public broker to simulate the ESP32/STM32 publishing the `STATUS` message after receiving a `CMD` to verify the state transitions and loading states.
