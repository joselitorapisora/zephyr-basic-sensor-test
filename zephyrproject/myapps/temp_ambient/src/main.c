#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

int main(void)
{
	const struct device *const mcp9808_1 = DEVICE_DT_GET(DT_ALIAS(my_mcp9808_1));
	const struct device *const mcp9808_2 = DEVICE_DT_GET(DT_ALIAS(my_mcp9808_2));
	const struct device *const opt3001_1 = DEVICE_DT_GET(DT_ALIAS(my_opt3001_1));
	int ret;

    // Check if the MCP9808 has been initialized (init function called)
	if (!device_is_ready(mcp9808_1)) {
		printf("Device %s is not ready.\n", mcp9808_1->name);
		return 0;
	}
    if (!device_is_ready(mcp9808_2)) {
		printf("Device %s is not ready.\n", mcp9808_2->name);
		return 0;
	}
    if (!device_is_ready(opt3001_1)) {
		printf("Device %s is not ready.\n", opt3001_1->name);
		return 0;
	}

    while (1) 
    {
        struct sensor_value tmp1;
        struct sensor_value tmp2;
        struct sensor_value amb1;

        // Fetch the temperature value from the sensor into the device's data structure
        ret = sensor_sample_fetch(mcp9808_1);
        ret |= sensor_sample_fetch(mcp9808_2);
        ret |= sensor_sample_fetch(opt3001_1);
        if (ret != 0) {
            printf("Sample fetch error: %d\n", ret);
            return 0;
        }
                
        // Copy the temperature value from the device's data structure into the tmp struct
        ret = sensor_channel_get(mcp9808_1, SENSOR_CHAN_AMBIENT_TEMP, &tmp1);
        ret |= sensor_channel_get(mcp9808_2, SENSOR_CHAN_AMBIENT_TEMP, &tmp2);
        ret |= sensor_channel_get(opt3001_1, SENSOR_CHAN_LIGHT, &amb1);
        if (ret != 0) {
            printf("Channel get error: %d\n", ret);
            return 0;
        }

        // Print the temperature value
        printf("Temp Zone1: %d.%02d *C\tTemp Zone2: %d.%02d *C\t Ambient Light: %d.%02d lux\n", \
            tmp1.val1, tmp1.val2/10000, tmp2.val1, tmp2.val2/10000, amb1.val1, amb1.val2/10000);
        k_sleep(K_SECONDS(2));
    }
    return 0;
}