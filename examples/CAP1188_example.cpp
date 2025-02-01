#include <Arduino.h>
#include "CAP1188.h"

// Create an instance
CAP1188 cap;

// or if you have a custom address:
// CAP1188 cap(0x28, Wire);

void setup()
{
    Serial.begin(115200);
    Wire.begin(); // for ESP32, can do Wire.begin(SDA, SCL);

    if (!cap.begin())
    {
        Serial.println("CAP1188 not found!");
        while (1)
            delay(10);
    }
    Serial.println("CAP1188 found.");

    // Set up sensitivity (0 => 128x, 7 => 1x)
    cap.setSensitivity(0); // super sensitive

    // Set all thresholds
    cap.setAllThresholds(0x40); // moderate threshold

    // Force a calibration on all 8 channels
    cap.forceCalibration(0xFF);

    // Link LED1 to CS1
    cap.linkLEDtoCS(1, true);
    // Make LED1 "Breathe"
    cap.setLEDBehavior(1, LED_BEHAVIOR_BREATHE);

    // For demonstration, let's set default duty cycles for breathe
    // e.g. 0xF0 => max=100%, min=77%
    cap.setLEDBreatheDuty(0xF0);

    // If you want the LED to be push-pull:
    cap.setLEDOutputType(1, true);

    Serial.println("Setup complete.");
}

void loop()
{
    // Quick poll of the sensor status
    uint8_t status = cap.getTouchStatus();

    if (status)
    { // Any channel touched?
        Serial.print("Touch detected! Status=0x");
        Serial.println(status, HEX);
        // Clear the interrupt so new events can fire
        cap.clearInterrupt();
    }

    delay(200);
}