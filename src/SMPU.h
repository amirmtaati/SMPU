#ifndef SMPU_H
#define SMPU_H

#include <Arduino.h>
#include <Wire.h>

typedef uint8_t   u8;
typedef uint16_t unsigned16;
typedef uint32_t u32;
typedef int8_t    s8;
typedef int16_t  s16;
typedef int32_t  s32;

#define MPU6050_ADDR_LOW          0x68
#define MPU6050_ADDR_HIGH         0x69
#define MPU6050_ADDRESS           0x68
#define MPU6050_REG_PWR_MGMT_1    0x6B
#define MPU6050_REG_WHO_AM_I      0x75
#define MPU6050_REG_ACCEL_XOUT_H  0x3B
#define MPU6050_REG_GYRO_XOUT_H   0x43
#define MPU6050_REG_TEMP_OUT_H    0x41
#define MPU6050_REG_CONFIG        0x1A
#define MPU6050_REG_GYRO_CONFIG   0x1B
#define MPU6050_REG_ACCEL_CONFIG  0x1C
#define MPU6050_WHO_AM_I_VALUE    0x68

class SMPU {
public:
  SMPU(u8 address = MPU6050_ADDR_LOW);

  bool begin();
  bool isConntected();
  void setSleep(bool);
  void setAccelRange();
  void setGyroRange();
  void readRaw();
  
private:
  u8 _address;

  bool writeByte(u8 reg, u8 val);
  bool readByte(u8 reg, u8& val);
};

#endif
