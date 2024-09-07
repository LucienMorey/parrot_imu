#ifndef ICM_CONSTANTS_H
#define ICM_CONSTANTS_H

const double GRAVITY = 9.81;

namespace ACCEL_CONVERSION_FACTORS
{
  const double ACCEL_2G = 2.0 / 32768.0;
  const double ACCEL_4G = 4.0 / 32768.0;
  const double ACEEL_8G = 8.0 / 32768.0;
  const double ACCEL_16G = 16.0 / 32768.0;
};

namespace GYRO_CONVERSION_FACTORS
{
  const double GYRO_250_DPS = 250.0 / 32768.0;
  const double GYRO_500_DPS = 500.0 / 32768.0;
  const double GYRO_1000_DPS = 1000.0 / 32768.0;
  const double GYRO_2000_DPS = 2000.0 / 32768.0;
};


namespace ORIENTATION_CONVERSION_FACTORS
{
  const double QUATERNION_9_DOF = 1.0 / 1073741824.0;
};


#endif