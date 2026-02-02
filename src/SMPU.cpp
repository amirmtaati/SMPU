#include "SMPU.h"

SMPU::SMPU(u8 address)
  : _address(address) {}

bool SMPU::writeByte(u8 reg, u8 val) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.write(val);

  return (Wire.endTransmission() == 0);
}

bool SMPU::readByte(u8 reg, u8& val) {
  Wire.beginTransmission(_address);
  Wire.write(reg);

  if (Wire.endTransmission(false) != 0)
    return false;

  if (Wire.requestFrom((int)_address, 1) != 1)
    return false;

  val = Wire.read();
  return true;
}

bool SMPU::readBytes(u8 reg, u8* buffer, u8 len) {
  Wire.beginTransmission(_address);
  Wire.write(reg);

  if (Wire.endTransmission(false) != 0)
    return false;

  if (Wire.requestFrom((int)_address, (int)len) != len)
    return false;

  for (u8 i = 0; i < len; i++) {
    buffer[i] = Wire.read();
  }

  return true;
}


bool SMPU::isConnected() {
  u8 val;
  if (!readByte(MPU6050_REG_WHO_AM_I, val))
    return false;

  return (val == MPU6050_WHO_AM_I_VALUE);
}

bool SMPU::wakeUp() {
  return writeByte(MPU6050_REG_PWR_MGMT_1, 0x00);
}

bool SMPU::begin() {
  Wire.begin();

  if (!isConnected())
    return false;

  if (!wakeUp())
    return false;

  delay(100);

  return true;
}

void SMPU::setAccelRange(AccelRange range) {
  u8 configValue;
  if (!readByte(MPU6050_REG_ACCEL_CONFIG, configValue))
    return;

  configValue = writeBits(configValue, MPU6050_ACCEL_RANGE_BIT_START,
			  MPU6050_ACCEL_RANGE_BIT_LENGTH,
			  range);

  writeByte(MPU6050_REG_ACCEL_CONFIG, configValue);
}

void SMPU::setGyroRange(GyroRange range) {
  u8 configValue;
  if (!readByte(MPU6050_REG_GYRO_CONFIG, configValue))
    return;

  configValue = writeBits(configValue, MPU6050_GYRO_RANGE_BIT_START,
			  MPU6050_GYRO_RANGE_BIT_LENGTH,
			  range);

  writeByte(MPU6050_REG_GYRO_CONFIG, configValue);
}

bool SMPU::readRawAccel() {
  u8 rawAccel[6];
  if (!readBytes(MPU6050_REG_ACCEL_XOUT_H, rawAccel, (u8)6))
    return false;

  _raw.accelX = (rawAccel[0] << 8) | rawAccel[1];
  _raw.accelY = (rawAccel[2] << 8) | rawAccel[3];
  _raw.accelZ = (rawAccel[4] << 8) | rawAccel[5];

  return true;
}

bool SMPU::readGyroAccel() {
  u8 rawGyro[6];
  if (!readBytes(MPU6050_REG_GYRO_XOUT_H, rawGyro, (u8)6))
    return false;

  _raw.gyroX = (rawGyro[0] << 8) | rawGyro[1];
  _raw.gyroY = (rawGyro[2] << 8) | rawGyro[3];
  _raw.gyroZ = (rawGyro[4] << 8) | rawGyro[5];

  return true;
}
