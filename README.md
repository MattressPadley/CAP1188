# CAP1188 Capacitive Touch Sensor Library

This library provides an interface for the CAP1188 capacitive touch sensor over I2C. The CAP1188 features 8 capacitive touch inputs and corresponding LED outputs with various animation capabilities.

I couldn't get the Adafruit library to work on my ESP32, so I wrote my own. (with an AI)

## Installation

Include this library in your project and make sure you have the Wire (I2C) library available.

## Basic Usage

```cpp
#include "CAP1188.h"

// Create instance with default I2C address (0x29)
CAP1188 cap;

void setup() {
    // Initialize the sensor
    if (!cap.begin()) {
        // Handle initialization failure
    }
}
```

## API Reference

### Constructor

#### `CAP1188(uint8_t i2cAddr = 0x29, TwoWire &wirePort = Wire)`
- Creates a new CAP1188 instance
- Parameters:
  - `i2cAddr`: I2C address of the device (default: 0x29)
  - `wirePort`: TwoWire instance for I2C communication (default: Wire)

### Initialization

#### `bool begin()`
- Initializes the device and checks if it's responding correctly
- Returns: `true` if device is found and initialized successfully, `false` otherwise

### Touch Sensing

#### `uint8_t getTouchStatus()`
- Gets the current touch status of all inputs
- Returns: 8-bit value where each bit represents a touch input (1 = touched)

#### `bool isTouched(uint8_t channel)`
- Checks if a specific channel is being touched
- Parameters:
  - `channel`: Channel number (1-8)
- Returns: `true` if the channel is touched, `false` otherwise

#### `void clearInterrupt()`
- Clears the interrupt flag
- Should be called after reading touch status to reset the interrupt

### Input Configuration

#### `void enableAllInputs(bool en)`
- Enables or disables all touch inputs
- Parameters:
  - `en`: `true` to enable all inputs, `false` to disable

#### `void setThreshold(uint8_t channel, uint8_t threshold)`
- Sets the touch sensitivity threshold for a specific channel
- Parameters:
  - `channel`: Channel number (1-8)
  - `threshold`: Threshold value

#### `void setAllThresholds(uint8_t threshold)`
- Sets the same touch sensitivity threshold for all channels
- Parameters:
  - `threshold`: Threshold value to apply to all channels

#### `void setSensitivity(uint8_t level)`
- Sets the overall sensitivity level
- Parameters:
  - `level`: Sensitivity level (0-7), where 0 = 128x and 7 = 1x

#### `void forceCalibration(uint8_t channelMask)`
- Forces calibration on specified channels
- Parameters:
  - `channelMask`: Bit mask of channels to calibrate

### LED Control

#### `void setLEDOutputType(uint8_t led, bool pushPull)`
- Sets the LED output type
- Parameters:
  - `led`: LED number (1-8)
  - `pushPull`: `true` for push-pull, `false` for open-drain

#### `void linkLEDtoCS(uint8_t led, bool link)`
- Links/unlinks an LED to its corresponding touch sensor
- Parameters:
  - `led`: LED number (1-8)
  - `link`: `true` to link, `false` to unlink

#### `void setLEDBehavior(uint8_t led, CAP1188_LEDBehavior behavior)`
- Sets the behavior mode for an LED
- Parameters:
  - `led`: LED number (1-8)
  - `behavior`: One of:
    - `LED_BEHAVIOR_DIRECT`: Direct control
    - `LED_BEHAVIOR_PULSE1`: Pulse pattern 1
    - `LED_BEHAVIOR_PULSE2`: Pulse pattern 2
    - `LED_BEHAVIOR_BREATHE`: Breathing pattern

#### `void setLEDPulse1Duty(uint8_t duty)`
- Sets the duty cycle for Pulse1 pattern
- Parameters:
  - `duty`: Duty cycle value (top nibble = max duty, bottom nibble = min duty)

#### `void setLEDPulse2Duty(uint8_t duty)`
- Sets the duty cycle for Pulse2 pattern
- Parameters:
  - `duty`: Duty cycle value

#### `void setLEDBreatheDuty(uint8_t duty)`
- Sets the duty cycle for Breathe pattern
- Parameters:
  - `duty`: Duty cycle value

#### `void setLEDDirectDuty(uint8_t duty)`
- Sets the duty cycle for Direct LED control
- Parameters:
  - `duty`: Duty cycle value

#### `void setLEDOutput(uint8_t led, bool on)`
- Directly controls LED state when in direct mode
- Parameters:
  - `led`: LED number (1-8)
  - `on`: `true` to turn on, `false` to turn off

## Example

```cpp
#include "CAP1188.h"

CAP1188 cap;

void setup() {
    Serial.begin(115200);
    
    if (!cap.begin()) {
        Serial.println("CAP1188 not found!");
        while (1);
    }
    
    // Configure touch sensitivity
    cap.setAllThresholds(0x20);
    cap.setSensitivity(4);
    
    // Setup LED behavior
    for (int i = 1; i <= 8; i++) {
        cap.linkLEDtoCS(i, true);
        cap.setLEDBehavior(i, LED_BEHAVIOR_DIRECT);
    }
}

void loop() {
    uint8_t touched = cap.getTouchStatus();
    
    for (int i = 0; i < 8; i++) {
        if (touched & (1 << i)) {
            Serial.print("Input ");
            Serial.print(i + 1);
            Serial.println(" touched");
        }
    }
    
    cap.clearInterrupt();
    delay(100);
}
