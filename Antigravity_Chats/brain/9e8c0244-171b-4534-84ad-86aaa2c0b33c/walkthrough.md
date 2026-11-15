# WhatsApp Installation Walkthrough

Since official native support for WhatsApp on Linux is limited, we explored two main methods: Snap and PWA. The PWA (Progressive Web App) method via Google Chrome was chosen for its reliability and ease of use.

## Steps Taken

### 1. Environment Check
*   Verified OS: Ubuntu 24.04.4 LTS.
*   Verified Browser: Google Chrome is installed at `/usr/bin/google-chrome`.

### 2. PWA Installation (User Action)
*   **Access**: Open [web.whatsapp.com](https://web.whatsapp.com) in Chrome.
*   **Link Device**: Scan the QR code with your phone.
*   **Install**: Use the Chrome menu (**Three dots** > **Save and Share** > **Install page as app...**) to create a standalone WhatsApp window.

## Expected Outcome
*   A "WhatsApp" icon will appear in your **Activities** / **Show Applications** menu.
*   The application will run in its own window, separate from the browser tabs.
*   Desktop notifications will work if enabled in setttings.

## Verification
*   [ ] WhatsApp icon is visible in the applications menu.
*   [ ] WhatsApp opens in a dedicated window.
*   [ ] Messages are received and notifications appear.

> [!TIP]
> You can pin the WhatsApp icon to your Ubuntu Dock by right-clicking it while it's open and selecting "Add to Favorites".
