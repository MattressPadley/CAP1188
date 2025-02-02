#pragma once

#include <Arduino.h>
#include <Wire.h>

// Default CAP1188 I2C Address (depends on ADDR_COMM pin resistor)
#ifndef CAP1188_I2C_ADDRESS
#define CAP1188_I2C_ADDRESS 0x29 // Common address
#endif

// Register Map (not exhaustive; add more if needed)
#define REG_MAIN_CONTROL 0x00
#define REG_GENERAL_STATUS 0x02
#define REG_SENSOR_INPUT_STATUS 0x03
#define REG_NOISE_FLAG_STATUS 0x0A

// Sensor data registers (raw values)
#define REG_SENSOR_1_DELTA 0x10
#define REG_SENSOR_2_DELTA 0x11
#define REG_SENSOR_3_DELTA 0x12
#define REG_SENSOR_4_DELTA 0x13
#define REG_SENSOR_5_DELTA 0x14
#define REG_SENSOR_6_DELTA 0x15
#define REG_SENSOR_7_DELTA 0x16
#define REG_SENSOR_8_DELTA 0x17

// Some config registers
#define REG_SENSITIVITY_CONTROL 0x1F
#define REG_CONFIGURATION 0x20
#define REG_SENSOR_INPUT_ENABLE 0x21
#define REG_SENSOR_INPUT_CONFIG 0x22
#define REG_SENSOR_INPUT_CONFIG2 0x23
#define REG_AVERAGING_CONFIG 0x24
#define REG_CALIBRATION_ACTIVATE 0x26
#define REG_INTERRUPT_ENABLE 0x27

// Recalibration config, etc.
#define REG_MULTIPLE_TOUCH_CONFIG 0x2A
#define REG_MULTIPLE_TOUCH_PATTERN 0x2D
#define REG_RECALIBRATION_CONFIG 0x2F

// Threshold registers for each input
// (0x30 ... 0x37). We'll define the base for indexing
#define REG_SENSOR_INPUT_THRESHOLD_1 0x30

// Standby (optional) ...
// #define REG_STANDBY_CHANNEL       0x40
// #define REG_STANDBY_CONFIGURATION 0x41
// #define REG_STANDBY_SENSITIVITY   0x42
// #define REG_STANDBY_THRESHOLD     0x43

// LED control registers
#define REG_LED_OUTPUT_TYPE 0x71
#define REG_SENSOR_INPUT_LED_LINK 0x72
#define REG_LED_POLARITY 0x73
#define REG_LED_OUTPUT_CONTROL 0x74

#define REG_LED_BEHAVIOR_1 0x81 // for LEDs 1..4
#define REG_LED_BEHAVIOR_2 0x82 // for LEDs 5..8

#define REG_LED_PULSE1_PERIOD 0x84
#define REG_LED_PULSE2_PERIOD 0x85
#define REG_LED_BREATHE_PERIOD 0x86

// Duty cycles for Pulse1, Pulse2, Breathe, Direct
#define REG_LED_PULSE1_DUTY_CYCLE 0x90
#define REG_LED_PULSE2_DUTY_CYCLE 0x91
#define REG_LED_BREATHE_DUTY_CYCLE 0x92
#define REG_LED_DIRECT_DUTY_CYCLE 0x93

// Product info
#define REG_PRODUCT_ID 0xFD
#define REG_MANUFACTURER_ID 0xFE
#define REG_REVISION 0xFF

// Some bitmasks
#define MAIN_CONTROL_INT 0x01 // Bit0: INT bit

// Example LED behavior bits (2 bits per LED)
enum CAP1188_LEDBehavior
{
    LED_BEHAVIOR_DIRECT = 0x00,
    LED_BEHAVIOR_PULSE1 = 0x01,
    LED_BEHAVIOR_PULSE2 = 0x02,
    LED_BEHAVIOR_BREATHE = 0x03
};

// A minimal CAP1188 driver for I2C
class CAP1188
{
public:
    // Constructor
    CAP1188(uint8_t i2cAddr = CAP1188_I2C_ADDRESS, TwoWire &wirePort = Wire);

    // Initialize device, returns true if found
    bool begin();

    // Basic Touch Reading
    uint8_t getTouchStatus();        // returns bits: 1 means touched
    bool isTouched(uint8_t channel); // channel in [1..8]

    // Raw value reading
    int8_t getRawValue(uint8_t channel); // Get raw delta count for a channel [1..8]

    // Clear interrupt (INT bit)
    void clearInterrupt();

    // Set All Channels Enabled/Disabled
    void enableAllInputs(bool en);

    // Enable multiple touch detection
    void enableMultiTouch(bool enable);

    // Set threshold for a specific channel [1..8]
    void setThreshold(uint8_t channel, uint8_t threshold);

    // Set threshold for ALL channels at once
    void setAllThresholds(uint8_t threshold);

    // Set sensitivity (DELTA_SENSE in reg 0x1F)
    // range 0..7 => 128x..1x
    void setSensitivity(uint8_t level);

    // Force Calibration on desired channels (mask)
    void forceCalibration(uint8_t channelMask);

    // --------------- LED Functionality ---------------
    // Set LED output type (push-pull = true, open-drain = false)
    void setLEDOutputType(uint8_t led, bool pushPull);

    // Link LEDx to CSx automatically
    void linkLEDtoCS(uint8_t led, bool link);

    // Set LED Behavior (Direct, Pulse1, Pulse2, Breathe)
    void setLEDBehavior(uint8_t led, CAP1188_LEDBehavior behavior);

    // Set LED Duty (min & max) for different modes
    // e.g. setLEDPulse1Duty(0xF0) => max=100%, min=77%
    void setLEDPulse1Duty(uint8_t duty);
    void setLEDPulse2Duty(uint8_t duty);
    void setLEDBreatheDuty(uint8_t duty);
    void setLEDDirectDuty(uint8_t duty);

    // Write to the LED Output Control register for manual on/off
    // (only relevant if LED is not linked or set to direct)
    void setLEDOutput(uint8_t led, bool on);

private:
    // I2C read/write
    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);
    // For potential expansions, block read/writes can be added.

    uint8_t _i2cAddr;
    TwoWire *_wire;
};