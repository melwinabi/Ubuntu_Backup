#include <WiFi.h>
#include <Audio.h>

// Define the pins for the ESP32-2432S024
#define I2S_DOUT      26  // The Audio Pin on the CYD
#define I2S_BCLK      -1  // Not used for internal DAC
#define I2S_LRCK      -1  // Not used for internal DAC

Audio audio;

void setup() {
    Serial.begin(115200);
    WiFi.begin("Your_SSID", "Your_Password");

    while (WiFi.status() != WL_CONNECTED) delay(500);
    
    // Connect the audio object to the internal DAC of GPIO 26
    audio.setInternalDAC(true);
    audio.setPinout(I2S_BCLK, I2S_LRCK, I2S_DOUT);
    audio.setVolume(15); // 0...21

    // Trigger Text to Speech (English)
    audio.connecttospeech("Hello! I am your ESP32 display board.", "en");
}

void loop() {
    audio.loop();
}