# CAP1188 Library for PlatformIO

This library provides an interface for the CAP1188 capacitive touch sensor over I2C. It allows you to initialize the sensor, read sensor data, and configure the sensor.

## Features
- Initialize the CAP1188 sensor
- Read sensor data
- Configure the sensor

## Installation
1. Clone the repository:
    ```
    git clone https://github.com/MattressPadley/CAP1188.git
    ```
2. Open the project in PlatformIO.

## Usage
1. Include the library in your project:
    ```cpp
    #include <CAP1188.h>
    ```
2. Initialize the CAP1188 object with the I2C address:
    ```cpp
    CAP1188 capSensor(0x28); // Example I2C address
    ```
3. Call the `begin` method to initialize the sensor:
    ```cpp
    capSensor.begin();
    ```
4. Read sensor data using the `readSensorData` method:
    ```cpp
    uint8_t sensorData = capSensor.readSensorData();
    ```
5. Configure the sensor using the `configureSensor` method:
    ```cpp
    capSensor.configureSensor(0x01); // Example configuration value
    ```

## Example
Here is an example sketch demonstrating the usage of the CAP1188 library:
```cpp
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
```

## API Documentation
### CAP1188 Class
- `CAP1188(uint8_t address)`: Constructor that initializes the CAP1188 object with the given I2C address.
- `void begin()`: Initializes the CAP1188 sensor.
- `uint8_t readSensorData()`: Reads and returns the sensor data.
- `void configureSensor(uint8_t config)`: Configures the sensor with the given configuration value.
