#ifndef SMPU_H
#define SMPU_H

#include <Arduino.h>
#include <Wire.h>

typedef uint8_t   u8;
typedef uint32_t u32;
typedef int8_t    s8;
typedef int16_t  s16;
typedef int32_t  s32;

struct RawData {
  s16 accelX, accelY, accelZ;
  s16 gyroX, gyroY, gyroZ;
  s16 temp;
};

struct ScaledData {
  float accelX, accelY, accelZ;
  float gyroX, gyroY, gyroZ;
  float temp;
};

struct Offset {
  float accelX, accelY, accelZ;
  float gyroX, gyroY, gyroZ;
  float temp;
};

#define MPU6050_ADDR_LOW          0x68
#define MPU6050_ADDR_HIGH         0x69
#define MPU6050_REG_PWR_MGMT_1    0x6B
#define MPU6050_REG_WHO_AM_I      0x75
#define MPU6050_REG_ACCEL_XOUT_H  0x3B
#define MPU6050_REG_GYRO_XOUT_H   0x43
#define MPU6050_REG_TEMP_OUT_H    0x41
#define MPU6050_REG_CONFIG        0x1A
#define MPU6050_REG_GYRO_CONFIG   0x1B
#define MPU6050_REG_ACCEL_CONFIG  0x1C
#define MPU6050_WHO_AM_I_VALUE    0x68

#define MPU6050_ACCEL_RANGE_BIT_START  3
#define MPU6050_ACCEL_RANGE_BIT_LENGTH 2
#define MPU6050_GYRO_RANGE_BIT_START   3
#define MPU6050_GYRO_RANGE_BIT_LENGTH  2
#define MPU6050_PWR_SLEEP_BIT          6

enum AccelRange {
  ACCEL_RANGE_2G  = 0,
  ACCEL_RANGE_4G  = 1,
  ACCEL_RANGE_8G  = 2,
  ACCEL_RANGE_16G = 3
};

enum GyroRange {
  GYRO_RANGE_250  = 0,
  GYRO_RANGE_500  = 1,
  GYRO_RANGE_1000 = 2,
  GYRO_RANGE_2000 = 3
};

inline u8 setBit(u8 val, u8 pos) {
  return val | (1 << pos);
}

inline u8 clearBit(u8 val, u8 pos) {
  return val & ~(1 << pos);
}

inline bool readBit(u8 val, u8 pos) {
  return (val >> pos) & 1;
}

inline u8 readBits(u8 val, u8 start, u8 len) {
  u8 mask = (1 << len) - 1;
  return (val >> start) & mask;
}

inline u8 writeBits(u8 val, u8 start, u8 len, u8 data) {
  u8 mask = ((1 << len) - 1) << start;
  val &= ~mask;
  val |= (data << start);
  return val;
}

class SMPU {
public:
  SMPU(u8 address = MPU6050_ADDR_LOW);

  bool begin();
  bool isConnected();
  bool wakeUp();
  void setAccelRange(AccelRange range);
  void setGyroRange(GyroRange range);
  bool readRawAccel();
  bool readRawGyro();
  bool readRawData();
  bool readData();

  inline s16 getRawAccelX() { return _raw.accelX; }
  inline s16 getRawAccelY() { return _raw.accelY; }
  inline s16 getRawAccelZ() { return _raw.accelZ; }
  inline s16 getRawGyroX()  { return _raw.gyroX;  }
  inline s16 getRawGyroY()  { return _raw.gyroY;  }
  inline s16 getRawGyroZ()  { return _raw.gyroZ;  }
  inline s16 getRawTemp()   { return _raw.temp;   }

  inline float getAccelX()  { return _scaled.accelX; }
  inline float getAccelY()  { return _scaled.accelY; }
  inline float getAccelZ()  { return _scaled.accelZ; }
  inline float getGyroX()   { return _scaled.gyroX;  }
  inline float getGyroY()   { return _scaled.gyroY;  }
  inline float getGyroZ()   { return _scaled.gyroZ;  }
  inline float getTemp()    { return _scaled.temp;   }

  bool calibrate();
  
private:
  u8         _address;
  u8         _accelRange;
  u8         _gyroRange;
  s16        _sampleSize;
  RawData    _raw;
  ScaledData _scaled;
  Offset     _offset;

  bool writeByte(u8 reg, u8 val);
  bool readByte(u8 reg, u8& val);
  bool readBytes(u8 reg, u8* buffer, u8 len);
  bool setOffset();
  float getAccelScale();
  float getGyroScale();
  void convertToScaled();
  void convertAccelToScaled();
  void convertGyroToScaled();
  void convertTempToScaled();
};

#endif
