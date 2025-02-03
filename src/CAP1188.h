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
/**
 * @enum CAP1188_LEDBehavior
 * @brief LED behavior modes for the CAP1188.
 */
enum CAP1188_LEDBehavior
{
    LED_BEHAVIOR_DIRECT = 0x00,  /**< Direct control */
    LED_BEHAVIOR_PULSE1 = 0x01,  /**< Pulse pattern 1 */
    LED_BEHAVIOR_PULSE2 = 0x02,  /**< Pulse pattern 2 */
    LED_BEHAVIOR_BREATHE = 0x03  /**< Breathing pattern */
};

// A minimal CAP1188 driver for I2C
/**
 * @class CAP1188
 * @brief A class to interface with the CAP1188 capacitive touch sensor.
 */
class CAP1188
{
public:
    /**
     * @brief Constructor for CAP1188 class.
     * @param i2cAddr I2C address of the CAP1188 device.
     * @param wirePort TwoWire instance for I2C communication.
     */
    CAP1188(uint8_t i2cAddr = CAP1188_I2C_ADDRESS, TwoWire &wirePort = Wire);

    /**
     * @brief Initializes the CAP1188 device.
     * @return true if the device is found and initialized successfully, false otherwise.
     */
    bool begin();

    /**
     * @brief Gets the touch status of all inputs.
     * @return 8-bit value where each bit represents a touch input (1 = touched).
     */
    uint8_t getTouchStatus();

    /**
     * @brief Checks if a specific channel is being touched.
     * @param channel Channel number (1-8).
     * @return true if the channel is touched, false otherwise.
     */
    bool isTouched(uint8_t channel);

    /**
     * @brief Gets the raw delta count for a specific channel.
     * @param channel Channel number (1-8).
     * @return Raw delta count value.
     */
    int8_t getRawValue(uint8_t channel);

    /**
     * @brief Clears the interrupt flag.
     */
    void clearInterrupt();

    /**
     * @brief Enables or disables all touch inputs.
     * @param en true to enable all inputs, false to disable.
     */
    void enableAllInputs(bool en);

    /**
     * @brief Enables or disables multiple touch detection.
     * @param enable true to enable multiple touch detection, false to disable.
     */
    void enableMultiTouch(bool enable);

    /**
     * @brief Sets the touch sensitivity threshold for a specific channel.
     * @param channel Channel number (1-8).
     * @param threshold Threshold value.
     */
    void setThreshold(uint8_t channel, uint8_t threshold);

    /**
     * @brief Sets the same touch sensitivity threshold for all channels.
     * @param threshold Threshold value to apply to all channels.
     */
    void setAllThresholds(uint8_t threshold);

    /**
     * @brief Sets the overall sensitivity level.
     * @param level Sensitivity level (0-7), where 0 = 128x and 7 = 1x.
     */
    void setSensitivity(uint8_t level);

    /**
     * @brief Forces calibration on specified channels.
     * @param channelMask Bit mask of channels to calibrate.
     */
    void forceCalibration(uint8_t channelMask);

    // --------------- LED Functionality ---------------
    /**
     * @brief Sets the LED output type.
     * @param led LED number (1-8).
     * @param pushPull true for push-pull, false for open-drain.
     */
    void setLEDOutputType(uint8_t led, bool pushPull);

    /**
     * @brief Links or unlinks an LED to its corresponding touch sensor.
     * @param led LED number (1-8).
     * @param link true to link, false to unlink.
     */
    void linkLEDtoCS(uint8_t led, bool link);

    /**
     * @brief Sets the behavior mode for an LED.
     * @param led LED number (1-8).
     * @param behavior LED behavior mode.
     */
    void setLEDBehavior(uint8_t led, CAP1188_LEDBehavior behavior);

    /**
     * @brief Sets the duty cycle for Pulse1 pattern.
     * @param duty Duty cycle value (top nibble = max duty, bottom nibble = min duty).
     */
    void setLEDPulse1Duty(uint8_t duty);

    /**
     * @brief Sets the duty cycle for Pulse2 pattern.
     * @param duty Duty cycle value.
     */
    void setLEDPulse2Duty(uint8_t duty);

    /**
     * @brief Sets the duty cycle for Breathe pattern.
     * @param duty Duty cycle value.
     */
    void setLEDBreatheDuty(uint8_t duty);

    /**
     * @brief Sets the duty cycle for Direct LED control.
     * @param duty Duty cycle value.
     */
    void setLEDDirectDuty(uint8_t duty);

    /**
     * @brief Directly controls LED state when in direct mode.
     * @param led LED number (1-8).
     * @param on true to turn on, false to turn off.
     */
    void setLEDOutput(uint8_t led, bool on);

private:
    /**
     * @brief Reads a value from a register.
     * @param reg Register address.
     * @return Value read from the register.
     */
    uint8_t readRegister(uint8_t reg);

    /**
     * @brief Writes a value to a register.
     * @param reg Register address.
     * @param value Value to write to the register.
     */
    void writeRegister(uint8_t reg, uint8_t value);

    uint8_t _i2cAddr; /**< I2C address of the CAP1188 device. */
    TwoWire *_wire;   /**< TwoWire instance for I2C communication. */
};
