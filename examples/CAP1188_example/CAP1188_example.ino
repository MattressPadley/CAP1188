#include <Wire.h>
#include <CAP1188.h>

CAP1188 capSensor(0x28); // Example I2C address

void setup() {
    Serial.begin(9600);
    capSensor.begin();
}

void loop() {
    uint8_t sensorData = capSensor.readSensorData();
    Serial.print("Sensor Data: ");
    Serial.println(sensorData);
    delay(1000);
}
