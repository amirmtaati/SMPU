#include "SMPU.h"

SMPU::SMPU(u8 address)
    : _address(address), _accelRange(0), _gyroRange(0), _sampleSize(500),
      _raw{}, _scaled{}, _offset{} {}

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

  setAccelRange(ACCEL_RANGE_2G);
  setGyroRange(GYRO_RANGE_250);

  return true;
}

void SMPU::setAccelRange(AccelRange range) {
  u8 configValue;
  if (!readByte(MPU6050_REG_ACCEL_CONFIG, configValue))
    return;

  configValue = writeBits(configValue, MPU6050_ACCEL_RANGE_BIT_START,
			  MPU6050_ACCEL_RANGE_BIT_LENGTH,
			  range);

  _accelRange = range;
  writeByte(MPU6050_REG_ACCEL_CONFIG, configValue);
}

void SMPU::setGyroRange(GyroRange range) {
  u8 configValue;
  if (!readByte(MPU6050_REG_GYRO_CONFIG, configValue))
    return;

  configValue = writeBits(configValue, MPU6050_GYRO_RANGE_BIT_START,
			  MPU6050_GYRO_RANGE_BIT_LENGTH,
			  range);

  _gyroRange = range;
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

bool SMPU::readRawGyro() {
  u8 rawGyro[6];
  if (!readBytes(MPU6050_REG_GYRO_XOUT_H, rawGyro, (u8)6))
    return false;

  _raw.gyroX = (rawGyro[0] << 8) | rawGyro[1];
  _raw.gyroY = (rawGyro[2] << 8) | rawGyro[3];
  _raw.gyroZ = (rawGyro[4] << 8) | rawGyro[5];

  return true;
}

bool SMPU::readRawData() {
  u8 buffer[14];

  if (!readBytes(MPU6050_REG_ACCEL_XOUT_H, buffer, (u8)14))
    return false;

  _raw.accelX = (buffer[0] << 8) | buffer[1];
  _raw.accelY = (buffer[2] << 8) | buffer[3];
  _raw.accelZ = (buffer[4] << 8) | buffer[5];

  _raw.temp = (buffer[6] << 8)   | buffer[7];

  _raw.gyroX = (buffer[8] << 8)  | buffer[9];
  _raw.gyroY = (buffer[10] << 8) | buffer[11];
  _raw.gyroZ = (buffer[12] << 8) | buffer[13];

  _raw.accelX -= _offset.accelX;
  _raw.accelY -= _offset.accelY;
  _raw.accelZ -= _offset.accelZ;

  _raw.gyroX  -= _offset.gyroX;
  _raw.gyroY  -= _offset.gyroY;
  _raw.gyroZ  -= _offset.gyroZ;

  return true;
}

bool SMPU::readData() {
  if (!readRawData())
    return false;

  convertToScaled();
  return true;
}

float SMPU::getAccelScale() {
    switch (_accelRange) {
    case ACCEL_RANGE_2G:
	return 16384.0;
    case ACCEL_RANGE_4G:
	return 8192.0;
    case ACCEL_RANGE_8G:
	return 4096.0;
    case ACCEL_RANGE_16G:
	return 2048.0;
    default:
	return 16384.0;
    }
}

float SMPU::getGyroScale() {
  switch (_gyroRange) {
  case GYRO_RANGE_250:
      return 131.0;
  case GYRO_RANGE_500:
      return 65.5;
  case GYRO_RANGE_1000:
      return 32.8;
  case GYRO_RANGE_2000:
      return 16.4;
  default:
      return 131.0;
  } 
}

void SMPU::convertToScaled() {
  convertAccelToScaled();
  convertGyroToScaled();
  convertTempToScaled();
}

void SMPU::convertAccelToScaled() {
    float accelScale = getAccelScale();

    _scaled.accelX = _raw.accelX / accelScale;
    _scaled.accelY = _raw.accelY / accelScale;
    _scaled.accelZ = _raw.accelZ / accelScale;
}

void SMPU::convertGyroToScaled() {
  float gyroScale = getGyroScale();

  _scaled.gyroX = _raw.gyroX / gyroScale;
  _scaled.gyroY = _raw.gyroY / gyroScale;
  _scaled.gyroZ = _raw.gyroZ / gyroScale;
}

void SMPU::convertTempToScaled() {
    _scaled.temp = (_raw.temp / 340.0) + 36.53;
}

bool SMPU::setOffset() {
  s16 accelX_sum = 0, accelY_sum = 0, accelZ_sum = 0;
  s16 gyroX_sum = 0, gyroY_sum = 0, gyroZ_sum = 0;

  for (int i = 0; i < _sampleSize; i++) {
    if (!readRawData())
	return false;

    accelX_sum += _raw.accelX;
    accelY_sum += _raw.accelY;
    accelZ_sum += _raw.accelZ;

    gyroX_sum  += _raw.gyroX; 
    gyroY_sum  += _raw.gyroY; 
    gyroZ_sum  += _raw.gyroZ;

    delay(2);
  }

  _offset.accelX = accelX_sum / _sampleSize;
  _offset.accelY = accelY_sum / _sampleSize;
  _offset.accelZ = (accelZ_sum / _sampleSize) - getAccelScale();

  _offset.gyroX  = gyroX_sum / _sampleSize;
  _offset.gyroY  = gyroY_sum / _sampleSize;
  _offset.gyroZ  = gyroZ_sum / _sampleSize;
    
  return true;
}

bool SMPU::calibrate() {
    return setOffset();
}
