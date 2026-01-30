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


bool SMPU::isConntected() {
  u8 val;
  if (!readByte(MPU6050_REG_WHO_AM_I, val))
    return false;

  return (val == MPU6050_WHO_AM_I_VALUE);
}

bool SMPU::begin() {
  Wire.begin();

  if (!isConntected())
    return false;

  return true;
}
