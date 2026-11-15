#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;
const int ALARM_PIN = 23; // DS3231 INT/SQW connected to GPIO23

void IRAM_ATTR alarmISR() {
    Serial.println("Alarm Triggered!");
}

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22); // SDA -> GPIO21, SCL -> GPIO22

    pinMode(ALARM_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ALARM_PIN), alarmISR, FALLING);

    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    if (rtc.lostPower()) {
        Serial.println("RTC lost power, setting time.");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set to compile time
    }

    rtc.clearAlarm(1);
    rtc.disableAlarm(2);
    rtc.writeSqwPinMode(DS3231_OFF); // Disable square wave output

    setNextAlarm();
}

void loop() {
    DateTime now = rtc.now();

    Serial.print("Time: ");
    Serial.print(now.hour());
    Serial.print(':');
    Serial.print(now.minute());
    Serial.print(':');
    Serial.println(now.second());

    if (rtc.alarmFired(1)) {
        Serial.println("ALARM 1 FIRED!");
        rtc.clearAlarm(1); // Reset alarm flag
        setNextAlarm(); // Schedule next alarm
    }

    delay(1000);
}

void setNextAlarm() {
    DateTime now = rtc.now();
    int nextHour = now.hour();
    int nextMinute = ((now.minute() / 15) + 1) * 15; // Round to next 15-minute mark

    if (nextMinute >= 60) {
        nextMinute = 0;
        nextHour++;
        if (nextHour >= 24) nextHour = 0; // Reset at midnight
    }

    DateTime nextAlarm(now.year(), now.month(), now.day(), nextHour, nextMinute, 0);
    rtc.setAlarm1(nextAlarm, DS3231_A1_Minute); // Match hour & minute

    Serial.print("Next alarm set for: ");
    Serial.print(nextHour);
    Serial.print(":");
    Serial.println(nextMinute);
}
