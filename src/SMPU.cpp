#include "SMPU.hpp"

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


bool SMPU::isConnected()() {
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
