# Smart Relay Switch App - Walkthrough

The Smart Relay Switch mobile-first app has been successfully built and designed to mirror a high-end, tactile physical switch. 

## Key Features Implemented

### 1. Robust MQTT Integration (`MQTTProvider`)
The application is wrapped in a custom `MQTTProvider` context (using `mqtt.js`) to ensure real-time, low-latency synchronization with the hardware.
- Connects to a public MQTT broker (using WebSockets) by default.
- Subscribes to `switch/1/state` for real-time status updates from the hardware.
- Publishes commands (`CMD: 1` or `CMD: 0`) to `switch/1/cmd`.
- Implements a precise state machine: **User Toggles UI -> Loading State -> Publish CMD -> Wait for hardware ACK via STATUS topic -> Finalize UI State**. This completely eliminates "Ghost States".
- A 3-second fallback timer ensures the UI reverts to its original state if the hardware fails to acknowledge the command.

### 2. Neumorphic / Glassmorphic UI 
The aesthetics use a carefully crafted dark Neumorphic theme to provide a tactile sensation on mobile devices.
- **Master Toggle (`MasterToggle.tsx`)**: The central button features inset and outset shadows to simulate depth. It transitions smoothly between Off (disconnected), Loading (processing), and On (glowing neon emerald) states.
- **Status Metadata (`StatusMetadata.tsx`)**: Displays real-time Uptime (tracked automatically), Signal Strength (derived from MQTT connection status), and Last Action timestamp.
- **Control Features (`ControlFeatures.tsx`)**: Includes a "Safety Lock" toggle to conceptually disable the physical switch, and a placeholder for an "Auto-Off Timer".
- **Real-Time Logging (`LoggingSection.tsx`)**: A scrollable terminal-like log that tracks the exact time, topic, and payload of the last 5 MQTT messages, making debugging incredibly easy.

## Technical Details

- **Framework**: Next.js 15 (App Router, TypeScript).
- **Styling**: Vanilla CSS tailored with specific Tailwind-like CSS variables for extreme control over drop-shadows and color transitions. The global background is set to a sleek `#1a1c23`.
- **Icons**: `lucide-react` library used for modern, clean scalable vector icons.

## How to Test

1. From the project directory, run:
   ```bash
   npm run dev
   ```
2. Open your web browser on your laptop or smartphone and navigate to `http://localhost:3000`.
3. Test the main toggle button. It will briefly go into "PROCESSING..." state before switching to "ACTIVE", simulating the real-world latency of an MQTT cycle.
4. Watch the "MQTT LOGS" section at the bottom populate with the payload data.

> [!TIP]
> To truly mirror it to your phone, ensure both devices are on the same Wi-Fi network. Find your laptop's local IP address (e.g., `192.168.1.100`) and navigate to `http://192.168.1.100:3000` on your phone browser. Add it to your home screen for a full-screen, native-app feel!
