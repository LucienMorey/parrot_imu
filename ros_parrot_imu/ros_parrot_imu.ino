#include <micro_ros_arduino.h>

#include "ICM_20948.h"
#include "SPI.h"
#include "constants.h"

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <sensor_msgs/msg/imu.h>

rcl_publisher_t publisher;
sensor_msgs__msg__Imu msg;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

// network setting variables
// Agent IP should be changed once used on real hardware to match network
const String AGENT_IP = "192.168.10.40";
const String CLIENT_IP = "192.168.10.10";
const int AGENT_PORT = 8888;
IPAddress client_ip;
IPAddress agent_ip;
// check if this can also be done based on the teensy ID
const byte MAC[] = {0x02, 0x47, 0x00, 0x00, 0x00, 0x01};

#define CS_PIN 0 // Which pin you connect CS to. Used only when "USE_SPI" is defined
#define LED_PIN 13

#define RCCHECK(fn)              \
  {                              \
    rcl_ret_t temp_rc = fn;      \
    if ((temp_rc != RCL_RET_OK)) \
    {                            \
      error_loop();              \
    }                            \
  }
#define RCSOFTCHECK(fn)          \
  {                              \
    rcl_ret_t temp_rc = fn;      \
    if ((temp_rc != RCL_RET_OK)) \
    {                            \
    }                            \
  }

void error_loop()
{
  while (1)
  {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

ICM_20948_SPI myICM;

void setup()
{
  client_ip.fromString(CLIENT_IP);
  agent_ip.fromString(AGENT_IP);
  // set ethernet as the ros transport
  set_microros_native_ethernet_udp_transports(MAC, client_ip, agent_ip, AGENT_PORT);

  delay(2000);

  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  SPI1.begin();
}

bool init_node_interfaces(void)
{

  bool success = true;

  allocator = rcl_get_default_allocator();

  // create init_options
  if (rclc_support_init(&support, 0, NULL, &allocator) != RCL_RET_OK)
  {
    success = false;
    Serial.printf("Failed to init ROS2 support\n");
  }

  if ((success == true) && (rclc_node_init_default(&node, "teensy_imu_node", "", &support)))
  {
    success = false;
    Serial.printf("Failed to init ROS2 node\n");
  }

  if ((success == true) && (rclc_publisher_init_default(
                               &publisher,
                               &node,
                               ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
                               "teensy_imu_data")))
  {
    success = false;
    Serial.printf("Failed to init ROS2 publisher\n");
  }

  memset(&msg, 0, sizeof(sensor_msgs__msg__Imu));

  return success;
}

void destroy_node_interfaces(void)
{
  rcl_publisher_fini(&publisher, &node);
  rcl_node_fini(&node);
  rclc_support_fini(&support);
}

bool configure_imu(void)
{
  bool initialized = false;
  while (!initialized)
  {

    // Initialize the ICM-20948
    // If the DMP is enabled, .begin performs a minimal startup. We need to configure the sample mode etc. manually.
    myICM.begin(CS_PIN, SPI1);

    if (myICM.status != ICM_20948_Stat_Ok)
    {
      delay(500);
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
    else
    {
      initialized = true;
    }
  }

  bool success = true;

  if (myICM.initializeDMP() != ICM_20948_Stat_Ok)
  {
    success = false;
    Serial.printf("Failure to initialise IMU DMP\n");
  }

  // Enable the required sensors
  if ((success == true) && (myICM.enableDMPSensor(INV_ICM20948_SENSOR_ORIENTATION) != ICM_20948_Stat_Ok))
  {
    success = false;
    Serial.printf("Failure to enable orientation sensor\n");
  }

  if ((success == true) && (myICM.enableDMPSensor(INV_ICM20948_SENSOR_GYROSCOPE) != ICM_20948_Stat_Ok))
  {
    success = false;
    Serial.printf("Failure to enable gyroscope sensor\n");
  }

  if ((success == true) && (myICM.enableDMPSensor(INV_ICM20948_SENSOR_ACCELEROMETER) != ICM_20948_Stat_Ok))
  {
    success = false;
    Serial.printf("Failure to enable accelerometer sensor\n");
  }

  // Configure the output data rate for each of the sensors
  // this should configure them to each run at the max speed
  if ((success == true) && (myICM.setDMPODRrate(DMP_ODR_Reg_Quat9, 0) != ICM_20948_Stat_Ok))
  {
    success = false;
    Serial.printf("Failure to configure orientation output data rate\n");
  }
  if ((success == true) && (myICM.setDMPODRrate(DMP_ODR_Reg_Accel, 0) != ICM_20948_Stat_Ok))
  {
    success = false;
    Serial.printf("Failure to configure accelerometer output data rate\n");
  }
  if ((success == true) && (myICM.setDMPODRrate(DMP_ODR_Reg_Gyro, 0) != ICM_20948_Stat_Ok))
  {
    success = false;
    Serial.printf("Failure to configure gyroscope output data rate\n");
  }

  ICM_20948_fss_t myFSS;
  myFSS.a = gpm8;
  myFSS.g = dps250;

  if ((success == true) && (myICM.setFullScale((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myFSS) != ICM_20948_Stat_Ok))
  {
    success = false;
    Serial.printf("Failure to set scale for Accelerometer and Gyroscope\n");
  }

  // Enable the FIFO
  if ((success == true) && (myICM.enableFIFO() != ICM_20948_Stat_Ok))
  {
    success = false;
    Serial.printf("Failure to enable FIFO\n");
  }

  // Enable the DMP
  if ((success == true) && (myICM.enableDMP() != ICM_20948_Stat_Ok))
  {
    success = false;
    Serial.printf("Failure to enable DMP\n");
  }

  // Reset DMP
  if ((success == true) && (myICM.resetDMP() != ICM_20948_Stat_Ok))
  {
    success = false;
    Serial.printf("Failure to reset DMP\n");
  }

  // Reset FIFO
  if ((success == true) && (myICM.resetFIFO() != ICM_20948_Stat_Ok))
  {
    success = false;
    Serial.printf("Failure to reset FIFO\n");
  }

  return success;
}

void loop()
{

  bool success = true;
  success = init_node_interfaces();

  if ((success == true) && (!configure_imu()))
  {
    success = false;
    Serial.printf("Failure to configure the imu\n");
  }

  while (success == true)
  {

    icm_20948_DMP_data_t data;
    myICM.readDMPdataFromFIFO(&data);

    if ((myICM.status == ICM_20948_Stat_Ok) || (myICM.status == ICM_20948_Stat_FIFOMoreDataAvail))
    {
      if ((data.header & DMP_header_bitmap_Quat9) > 0) // We have asked for orientation data so we should receive Quat9
      {
        // Q0 value is computed from this equation: Q0^2 + Q1^2 + Q2^2 + Q3^2 = 1.
        // In case of drift, the sum will not add to 1, therefore, quaternion data need to be corrected with right bias values.
        // The quaternion data is scaled by 2^30.
        // Scale to +/- 1

        msg.orientation.x = ((double)data.Quat9.Data.Q1) * ORIENTATION_CONVERSION_FACTORS::QUATERNION_9_DOF; // Convert to double. Divide by 2^30
        msg.orientation.y = ((double)data.Quat9.Data.Q2) * ORIENTATION_CONVERSION_FACTORS::QUATERNION_9_DOF; // Convert to double. Divide by 2^30
        msg.orientation.z = ((double)data.Quat9.Data.Q3) * ORIENTATION_CONVERSION_FACTORS::QUATERNION_9_DOF; // Convert to double. Divide by 2^30
        msg.orientation.w = sqrt(1.0 - ((msg.orientation.x * msg.orientation.x) + (msg.orientation.y * msg.orientation.y) + (msg.orientation.z * msg.orientation.z)));
      }

      if ((data.header & DMP_header_bitmap_Accel) > 0) // We have asked for acceleration so we should receive raw accel
      {
        msg.linear_acceleration.x = (float)data.Raw_Accel.Data.X * ACCEL_CONVERSION_FACTORS::ACEEL_8G * GRAVITY;
        msg.linear_acceleration.y = (float)data.Raw_Accel.Data.Y * ACCEL_CONVERSION_FACTORS::ACEEL_8G * GRAVITY;
        msg.linear_acceleration.z = (float)data.Raw_Accel.Data.Z * ACCEL_CONVERSION_FACTORS::ACEEL_8G * GRAVITY;
      }

      if ((data.header & DMP_header_bitmap_Gyro) > 0) // We have asked angular velocity so we should receive raw gyro
      {
        msg.angular_velocity.x = (float)data.Raw_Gyro.Data.X * GYRO_CONVERSION_FACTORS::GYRO_250_DPS * PI / 180.0;
        msg.angular_velocity.y = (float)data.Raw_Gyro.Data.Y * GYRO_CONVERSION_FACTORS::GYRO_250_DPS * PI / 180.0;
        msg.angular_velocity.z = (float)data.Raw_Gyro.Data.Z * GYRO_CONVERSION_FACTORS::GYRO_250_DPS * PI / 180.0;
      }
    }

    if (rcl_publish(&publisher, &msg, NULL) != RCL_RET_OK)
    {
      success = false;
    }
  }

  destroy_node_interfaces();
}
