#include "CAP1188.h"

/**
 * @brief Constructor for CAP1188 class.
 * @param i2cAddr I2C address of the CAP1188 device.
 * @param wirePort TwoWire instance for I2C communication.
 */
CAP1188::CAP1188(uint8_t i2cAddr, TwoWire &wirePort)
    : _i2cAddr(i2cAddr), _wire(&wirePort)
{
}

/**
 * @brief Initializes the CAP1188 device.
 * @return true if the device is found and initialized successfully, false otherwise.
 */
bool CAP1188::begin()
{
    _wire->begin();

    // Check product ID
    uint8_t prodID = readRegister(REG_PRODUCT_ID);
    uint8_t manID = readRegister(REG_MANUFACTURER_ID);

    if (prodID != 0x50 || manID != 0x5D)
    {
        // Not found or unexpected device
        return false;
    }

    // Optional: read revision
    // uint8_t rev = readRegister(REG_REVISION);

    // Clear INT if any
    clearInterrupt();

    // Enable all inputs by default
    enableAllInputs(true);

    // Enable multiple touch detection by default
    enableMultiTouch(true);

    // Example: set a default config if desired
    // e.g. enable digital noise filter, etc.
    // (REG_CONFIGURATION = 0x20 if you want some defaults)
    // You can poke around the bits if needed.

    return true;
}

/**
 * @brief Gets the touch status of all inputs.
 * @return 8-bit value where each bit represents a touch input (1 = touched).
 */
uint8_t CAP1188::getTouchStatus()
{
    // Returns the 8-bit value of which inputs are touched
    return readRegister(REG_SENSOR_INPUT_STATUS);
}

/**
 * @brief Checks if a specific channel is being touched.
 * @param channel Channel number (1-8).
 * @return true if the channel is touched, false otherwise.
 */
bool CAP1188::isTouched(uint8_t channel)
{
    if (channel < 1 || channel > 8)
        return false;
    uint8_t status = getTouchStatus();
    return (status & (1 << (channel - 1))) != 0;
}

/**
 * @brief Gets the raw delta count for a specific channel.
 * @param channel Channel number (1-8).
 * @return Raw delta count value.
 */
int8_t CAP1188::getRawValue(uint8_t channel)
{
    if (channel < 1 || channel > 8)
        return 0;
    
    // Read the delta count register for the specified channel
    // The delta registers start at 0x10 and go up to 0x17
    return (int8_t)readRegister(REG_SENSOR_1_DELTA + (channel - 1));
}

/**
 * @brief Clears the interrupt flag.
 */
void CAP1188::clearInterrupt()
{
    // Read current main control
    uint8_t r = readRegister(REG_MAIN_CONTROL);
    // Clear the INT bit (bit0)
    // This will de-assert ALERT# if there's no continuing condition
    writeRegister(REG_MAIN_CONTROL, r & ~MAIN_CONTROL_INT);
}

/**
 * @brief Enables or disables all touch inputs.
 * @param en true to enable all inputs, false to disable.
 */
void CAP1188::enableAllInputs(bool en)
{
    // Write 0xFF to enable all, or 0x00 to disable all
    writeRegister(REG_SENSOR_INPUT_ENABLE, en ? 0xFF : 0x00);
}

/**
 * @brief Sets the touch sensitivity threshold for a specific channel.
 * @param channel Channel number (1-8).
 * @param threshold Threshold value.
 */
void CAP1188::setThreshold(uint8_t channel, uint8_t threshold)
{
    if (channel < 1 || channel > 8)
        return;
    // Each input threshold is at 0x30 + (channel-1)
    writeRegister(REG_SENSOR_INPUT_THRESHOLD_1 + (channel - 1), threshold);
}

/**
 * @brief Sets the same touch sensitivity threshold for all channels.
 * @param threshold Threshold value to apply to all channels.
 */
void CAP1188::setAllThresholds(uint8_t threshold)
{
    // If you set the BUT_LD_TH bit, writing to channel1 threshold
    // can copy the same value to all channels automatically.
    // Alternatively, just loop over 1..8
    for (uint8_t ch = 1; ch <= 8; ch++)
    {
        setThreshold(ch, threshold);
    }
}

/**
 * @brief Sets the overall sensitivity level.
 * @param level Sensitivity level (0-7), where 0 = 128x and 7 = 1x.
 */
void CAP1188::setSensitivity(uint8_t level)
{
    // SENSITIVITY_CONTROL (0x1F)
    // bits [6:4] => DELTA_SENSE(2:0)
    // default is 0b100 => 32x
    // level 0 => 128x, 7 => 1x
    uint8_t reg = readRegister(REG_SENSITIVITY_CONTROL);
    reg &= ~0x70;               // Clear the old bits
    reg |= (level & 0x07) << 4; // Insert new
    writeRegister(REG_SENSITIVITY_CONTROL, reg);
}

/**
 * @brief Forces calibration on specified channels.
 * @param channelMask Bit mask of channels to calibrate.
 */
void CAP1188::forceCalibration(uint8_t channelMask)
{
    // Writing 1 bits to REG_CALIBRATION_ACTIVATE forces re-cal
    // for corresponding channels
    writeRegister(REG_CALIBRATION_ACTIVATE, channelMask);
    // The device auto-clears these bits after calibration
}

/**
 * @brief Sets the LED output type.
 * @param led LED number (1-8).
 * @param pushPull true for push-pull, false for open-drain.
 */
void CAP1188::setLEDOutputType(uint8_t led, bool pushPull)
{
    if (led < 1 || led > 8)
        return;
    // LED Output Type register (0x71), one bit per LED
    // 0 => open drain, 1 => push-pull
    uint8_t regVal = readRegister(REG_LED_OUTPUT_TYPE);
    uint8_t mask = 1 << (led - 1);
    if (pushPull)
        regVal |= mask;
    else
        regVal &= ~mask;
    writeRegister(REG_LED_OUTPUT_TYPE, regVal);
}

/**
 * @brief Links or unlinks an LED to its corresponding touch sensor.
 * @param led LED number (1-8).
 * @param link true to link, false to unlink.
 */
void CAP1188::linkLEDtoCS(uint8_t led, bool link)
{
    if (led < 1 || led > 8)
        return;
    // Sensor Input LED Linking (0x72)
    uint8_t regVal = readRegister(REG_SENSOR_INPUT_LED_LINK);
    uint8_t mask = 1 << (led - 1);
    if (link)
        regVal |= mask;
    else
        regVal &= ~mask;
    writeRegister(REG_SENSOR_INPUT_LED_LINK, regVal);
}

/**
 * @brief Sets the behavior mode for an LED.
 * @param led LED number (1-8).
 * @param behavior LED behavior mode.
 */
void CAP1188::setLEDBehavior(uint8_t led, CAP1188_LEDBehavior behavior)
{
    if (led < 1 || led > 8)
        return;

    // Each LED has 2 bits in either REG_LED_BEHAVIOR_1 (for LED1..4)
    // or REG_LED_BEHAVIOR_2 (for LED5..8)
    // LED1..4 are in 0x81 => bits [1:0] for LED1, [3:2] for LED2, ...
    // LED5..8 are in 0x82 => same pattern
    uint8_t regAddr = (led <= 4) ? REG_LED_BEHAVIOR_1 : REG_LED_BEHAVIOR_2;
    uint8_t shift = 2 * ((led - 1) % 4);

    uint8_t regVal = readRegister(regAddr);
    // Clear bits
    regVal &= ~(0x03 << shift);
    // Insert new behavior
    regVal |= ((uint8_t)behavior & 0x03) << shift;
    writeRegister(regAddr, regVal);
}

/**
 * @brief Sets the duty cycle for Pulse1 pattern.
 * @param duty Duty cycle value (top nibble = max duty, bottom nibble = min duty).
 */
void CAP1188::setLEDPulse1Duty(uint8_t duty)
{
    // The top nibble is max duty, bottom nibble min duty
    // 0xF0 => 100% max, 77% min by default decode
    // see data sheet Table 6-60 for decode details
    writeRegister(REG_LED_PULSE1_DUTY_CYCLE, duty);
}

/**
 * @brief Sets the duty cycle for Pulse2 pattern.
 * @param duty Duty cycle value.
 */
void CAP1188::setLEDPulse2Duty(uint8_t duty)
{
    writeRegister(REG_LED_PULSE2_DUTY_CYCLE, duty);
}

/**
 * @brief Sets the duty cycle for Breathe pattern.
 * @param duty Duty cycle value.
 */
void CAP1188::setLEDBreatheDuty(uint8_t duty)
{
    writeRegister(REG_LED_BREATHE_DUTY_CYCLE, duty);
}

/**
 * @brief Sets the duty cycle for Direct LED control.
 * @param duty Duty cycle value.
 */
void CAP1188::setLEDDirectDuty(uint8_t duty)
{
    writeRegister(REG_LED_DIRECT_DUTY_CYCLE, duty);
}

/**
 * @brief Directly controls LED state when in direct mode.
 * @param led LED number (1-8).
 * @param on true to turn on, false to turn off.
 */
void CAP1188::setLEDOutput(uint8_t led, bool on)
{
    // LED Output Control (0x74)
    // bit = 1 => LED is driven (actuated), bit = 0 => off
    if (led < 1 || led > 8)
        return;
    uint8_t regVal = readRegister(REG_LED_OUTPUT_CONTROL);
    uint8_t mask = 1 << (led - 1);
    if (on)
        regVal |= mask;
    else
        regVal &= ~mask;
    writeRegister(REG_LED_OUTPUT_CONTROL, regVal);
}

/**
 * @brief Enables or disables multiple touch detection.
 * @param enable true to enable multiple touch detection, false to disable.
 */
void CAP1188::enableMultiTouch(bool enable)
{
    // Read current multiple touch config
    uint8_t config = readRegister(REG_MULTIPLE_TOUCH_CONFIG);
    
    if (enable) {
        // Clear the multiple touch blocking bit (bit 7)
        config &= ~0x80;
    } else {
        // Set the multiple touch blocking bit
        config |= 0x80;
    }
    
    writeRegister(REG_MULTIPLE_TOUCH_CONFIG, config);
}

/**
 * @brief Reads a value from a register.
 * @param reg Register address.
 * @return Value read from the register.
 */
uint8_t CAP1188::readRegister(uint8_t reg)
{
    _wire->beginTransmission(_i2cAddr);
    _wire->write(reg);
    _wire->endTransmission(false);

    _wire->requestFrom((int)_i2cAddr, 1);
    if (_wire->available())
    {
        return _wire->read();
    }
    return 0xFF; // or throw an error, etc.
}

/**
 * @brief Writes a value to a register.
 * @param reg Register address.
 * @param value Value to write to the register.
 */
void CAP1188::writeRegister(uint8_t reg, uint8_t value)
{
    _wire->beginTransmission(_i2cAddr);
    _wire->write(reg);
    _wire->write(value);
    _wire->endTransmission();
}
