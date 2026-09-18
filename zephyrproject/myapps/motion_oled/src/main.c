#include <stdint.h>
#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/display.h>
#include <zephyr/display/cfb.h>
#include <zephyr/logging/log.h>
#include <zephyr/display/cfb.h>
#include "image.h"

#define DISPLAY_BUFFER_PITCH 132
uint8_t msgBuf[100];

static const struct device *display = DEVICE_DT_GET(DT_ALIAS(sh1107));
const struct device *const motionSensor = DEVICE_DT_GET(DT_ALIAS(mpu6050));
int ret;


void normalize_sensor_value(struct sensor_value *value) {
    if (value->val2 < 0) {
        // Handle negative fractional part
        value->val1 -= 1;                  // Decrement integer part
        value->val2 += 1000000;           // Add 1.0 to fractional part
    }
}


int main(void)
{
  // struct sensor_value motionTemp;
  struct sensor_value motionAcc[3];
  struct sensor_value motionGyr[3];

  // Check devices
  if (!device_is_ready(display))
  {
    printf("Device %s is not ready.\n", display->name);
    return 0;
  }
  if (!device_is_ready(motionSensor))
  {
    printf("Device %s is not ready.\n", motionSensor->name);
    return 0;
  }

  // Display welcome image for 3 sec
  struct display_capabilities capabilities;
  display_get_capabilities(display, &capabilities);
  const struct display_buffer_descriptor buf_desc = {
      .width = capabilities.x_resolution,
      .height = capabilities.y_resolution,
      .buf_size = capabilities.x_resolution * capabilities.y_resolution,
      .pitch = capabilities.x_resolution};
  if (display_write(display, 0, 0, &buf_desc, imageBuf) != 0)
  {
    printf("could not write to display");
  }
  k_sleep(K_MSEC(3000));

  // Initialize cfb framebuffer
  if (cfb_framebuffer_init(display) != 0)
  {
    printf("could not initialize CFB");
    return 0;
  }
  if (cfb_framebuffer_clear(display, true) != 0)
  {
    printf("could not clear display");
    return 0;
  }

  cfb_print(display, "Accel:", 0, 0);
  cfb_print(display, "Gyro:", 67, 0);
  cfb_framebuffer_finalize(display);

  while (1)
  {
    // // Fetch data from sensors
    ret = sensor_sample_fetch(motionSensor);
    if (ret)
    {
      printf("Sample fetch error: %d\n", ret);
      return 0;
    }

    // SENSOR_CHAN_ACCEL_XYZ
    // SENSOR_CHAN_ACCEL_X
    // SENSOR_CHAN_ACCEL_Y
    // SENSOR_CHAN_ACCEL_Z
    // SENSOR_CHAN_GYRO_XYZ
    // SENSOR_CHAN_GYRO_X
    // SENSOR_CHAN_GYRO_Y
    // SENSOR_CHAN_GYRO_Z
    // SENSOR_CHAN_DIE_TEMP

    ret = sensor_channel_get(motionSensor, SENSOR_CHAN_ACCEL_XYZ, motionAcc);
    if (ret)
    {
      printf("Channel get error: %d\n", ret);
      return 0;
    }

    ret = sensor_channel_get(motionSensor, SENSOR_CHAN_GYRO_XYZ, motionGyr);
    if (ret)
    {
      printf("Channel get error: %d\n", ret);
      return 0;
    }


    // // JMR: Writing spaces to framebuffer instead of doing a clear to avoid flicker...
    // cfb_framebuffer_clear(display, true);
    cfb_print(display, "               ", 0, 1 * 15);
    cfb_print(display, "               ", 0, 2 * 15);
    cfb_print(display, "               ", 0, 3 * 15);

    //sprintf((char *)msgBuf, "Temp: %d.%02d", motionTemp.val1, motionTemp.val2 / 10000);
    //cfb_print(display, msgBuf, 0, 0);
    normalize_sensor_value(&motionAcc[0]);
    sprintf((char *)msgBuf, "%d.%02d", motionAcc[0].val1, motionAcc[0].val2 / 10000);
    cfb_print(display, msgBuf, 0, 1 * 15);
    normalize_sensor_value(&motionAcc[1]);
    sprintf((char *)msgBuf, "%d.%02d", motionAcc[1].val1, motionAcc[1].val2 / 10000);
    cfb_print(display, msgBuf, 0, 2 * 15);
    normalize_sensor_value(&motionAcc[2]);
    sprintf((char *)msgBuf, "%d.%02d", motionAcc[2].val1, motionAcc[2].val2 / 10000);
    cfb_print(display, msgBuf, 0, 3 * 15);

    normalize_sensor_value(&motionGyr[0]);
    sprintf((char *)msgBuf, "%d.%02d", motionGyr[0].val1, motionGyr[0].val2 / 10000);
    cfb_print(display, msgBuf, 67, 1 * 15);
    normalize_sensor_value(&motionGyr[1]);
    sprintf((char *)msgBuf, "%d.%02d", motionGyr[1].val1, motionGyr[1].val2 / 10000);
    cfb_print(display, msgBuf, 67, 2 * 15);
    normalize_sensor_value(&motionGyr[2]);
    sprintf((char *)msgBuf, "%d.%02d", motionGyr[2].val1, motionGyr[2].val2 / 10000);
    cfb_print(display, msgBuf, 67, 3 * 15);

    cfb_framebuffer_finalize(display);
    k_sleep(K_MSEC(50));
  }
  return 0;
}