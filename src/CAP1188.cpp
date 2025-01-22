#include "CAP1188.h"
#include <Wire.h>

CAP1188::CAP1188(uint8_t address) : _address(address) {
    Wire.begin();
}

void CAP1188::begin() {
    // Initialize the sensor
    Wire.beginTransmission(_address);
    Wire.write(0x00); // Example register address
    Wire.write(0x01); // Example configuration value
    Wire.endTransmission();
}

uint8_t CAP1188::readSensorData() {
    Wire.beginTransmission(_address);
    Wire.write(0x01); // Example register address for reading data
    Wire.endTransmission();
    
    Wire.requestFrom(_address, (uint8_t)1);
    if (Wire.available()) {
        _sensorData = Wire.read();
    }
    return _sensorData;
}

void CAP1188::configureSensor(uint8_t config) {
    Wire.beginTransmission(_address);
    Wire.write(0x02); // Example register address for configuration
    Wire.write(config);
    Wire.endTransmission();
}
