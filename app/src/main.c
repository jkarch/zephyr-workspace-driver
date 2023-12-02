/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <app_version.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

//Set up
//https://github.com/nrfconnect/ncs-example-application/tree/main
//follow readme to initialize, after tool chain downloaded etc
//copy overlay for proper platform
//edit this main file
//west build -b nrf52dk_nrf52832 app -- -DDTC_OVERLAY_FILE=$(PWD)/boards/nrf52dk_nrf52832.overlay 


/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED1_NODE DT_ALIAS(led1)
#define MIN_PERIOD PWM_SEC(1U) / 128U
#define MAX_PERIOD PWM_SEC(1U)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));
static const struct device *iis = DEVICE_DT_GET(DT_INST(0,st_iis3wdb));
static const struct device *sensor = DEVICE_DT_GET(DT_NODELABEL(examplesensor0));







int main(void)
{
	uint32_t max_period;
	uint32_t period;
	uint8_t dir = 0U;
	int ret;
	uint16_t result;

	printk("PWM-based blinky\n");
	

	printk("Zephyr Example Application %s\n", APP_VERSION_STRING);
	if (!pwm_is_ready_dt(&pwm_led0)) {
		printk("Error: PWM device %s is not ready\n",
		       pwm_led0.dev->name);
		return 0;
	}

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	

    if (iis == NULL)
    {
        printk("Could not get the iis3wdb sensor \r\n");
    }
    if (!device_is_ready(iis))
    {
        printk("iis3wdb not ready \r\n");
    }
    //require API calls from Sensors API 

	
	if (!device_is_ready(sensor)) {
		LOG_ERR("Sensor not ready");
		return 0;
	}

	/*
	 * In case the default MAX_PERIOD value cannot be set for
	 * some PWM hardware, decrease its value until it can.
	 *
	 * Keep its value at least MIN_PERIOD * 4 to make sure
	 * the sample changes frequency at least once.
	 */
	printk("Calibrating for channel %d...\n", pwm_led0.channel);
	max_period = MAX_PERIOD;
	while (pwm_set_dt(&pwm_led0, max_period, max_period / 2U)) {
		max_period /= 2U;
		if (max_period < (4U * MIN_PERIOD)) {
			printk("Error: PWM device "
			       "does not support a period at least %lu\n",
			       4U * MIN_PERIOD);
			return 0;
		}
	}

	printk("Done calibrating; maximum/minimum periods %u/%lu nsec\n",
	       max_period, MIN_PERIOD);

	period = max_period;


	while (1) {
		struct sensor_value val,acc_x,acc_y,acc_z;
		ret = pwm_set_dt(&pwm_led0, period, period / 2U);
		if (ret) {
			printk("Error %d: failed to set pulse width\n", ret);
			return 0;
		}

		period = dir ? (period * 2U) : (period / 2U);
		if (period > max_period) {
			period = max_period / 2U;
			dir = 0U;
		} else if (period < MIN_PERIOD) {
			period = MIN_PERIOD * 2U;
			dir = 1U;
		}

		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return 0;
		}

		sensor_sample_fetch(iis);
        sensor_channel_get(iis,SENSOR_CHAN_ACCEL_X,&acc_x);
        sensor_channel_get(iis,SENSOR_CHAN_ACCEL_Y ,&acc_y);
        sensor_channel_get(iis,SENSOR_CHAN_ACCEL_Z ,&acc_z);
        printk("X: %dmg\r\n",acc_x.val1);
        printk("Y: %dmg\r\n",acc_y.val1);
        printk("Z: %dmg\r\n",acc_z.val1);
        printk("\r\n");
	

		ret = sensor_sample_fetch(sensor);
		if (ret < 0) {
			LOG_ERR("Could not fetch sample (%d)", ret);
			return 0;
		}

		ret = sensor_channel_get(sensor, SENSOR_CHAN_PROX, &val);
		if (ret < 0) {
			LOG_ERR("Could not get sample (%d)", ret);
			return 0;
		}

		printk("Sensor value: %d\n", val.val1);
		k_sleep(K_MSEC(100U));
	}
	return 0;
}

