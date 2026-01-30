# Simple MPU Library (SMPU)

A lightweight, easy-to-use Arduino library for the MPU6050 6-axis accelerometer and gyroscope sensor.

## Features

- ✅ Simple, intuitive API
- ✅ Efficient I2C communication
- ✅ Raw sensor data reading
- ✅ Configurable accelerometer and gyroscope ranges
- ✅ Low memory footprint
- ✅ Clean, readable code

## Hardware Requirements

- Arduino (Uno, Nano, Mega, ESP32, etc.)
- MPU6050 sensor module
- I2C connection (SDA, SCL)

## Wiring

| MPU6050 | Arduino Uno/Nano | ESP32 |
|---------|------------------|-------|
| VCC     | 3.3V or 5V       | 3.3V  |
| GND     | GND              | GND   |
| SCL     | A5               | GPIO 22 |
| SDA     | A4               | GPIO 21 |

## Installation

1. Download this repository as a ZIP file
2. In Arduino IDE: Sketch → Include Library → Add .ZIP Library
3. Select the downloaded ZIP file
4. Restart Arduino IDE

## Quick Start
```cpp
#include <SMPU.h>

SMPU mpu;

void setup() {
  Serial.begin(115200);
  
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }
  
  Serial.println("MPU6050 initialized!");
  
  // Optional: Configure sensor ranges
  mpu.setAccelRange(ACCEL_RANGE_2G);
  mpu.setGyroRange(GYRO_RANGE_250);
}

void loop() {
  if (mpu.readRawData()) {
    Serial.print("Accel X: ");
    Serial.print(mpu.getRawAccelX());
    Serial.print(" Y: ");
    Serial.print(mpu.getRawAccelY());
    Serial.print(" Z: ");
    Serial.println(mpu.getRawAccelZ());
  }
  
  delay(100);
}
```

## API Reference

### Initialization
```cpp
SMPU mpu;              // Create instance with default I2C address (0x68)
SMPU mpu(0x69);        // Use alternate I2C address

bool begin();          // Initialize sensor, returns true if successful
bool isConnected();    // Check if sensor is connected
```

### Configuration
```cpp
void setSleep(bool enable);              // Enable/disable sleep mode
void setAccelRange(AccelRange range);    // Set accelerometer range
void setGyroRange(GyroRange range);      // Set gyroscope range
```

