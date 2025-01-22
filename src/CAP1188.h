#ifndef CAP1188_H
#define CAP1188_H

#include <Wire.h>

class CAP1188 {
public:
    CAP1188(uint8_t address);
    void begin();
    uint8_t readSensorData();
    void configureSensor(uint8_t config);

private:
    uint8_t _address;
    uint8_t _sensorData;
};

#endif
