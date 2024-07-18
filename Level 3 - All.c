/***************************************************************************//**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
#include "sl_status.h"
#include "sl_sleeptimer.h"
#include "app_log.h"
#include "math.h"
#include "mikroe_mq7.h"

#define READING_INTERVAL_MSEC 3000
#include <em_gpio.h>
#include <ustimer.h>
#include <string.h>

// Dev. by: Phong

char DHT11Pin[] = "D3";

void pinMode(char pin[], char mode[]) { // two modes: input and output
    if (!(strcmp(mode, "in"))) {
        GPIO_PinModeSet(pin[0] - 'A', pin[1] - '0', gpioModeInput, 0);
    } else if (!(strcmp(mode, "out"))) {
        GPIO_PinModeSet(pin[0] - 'A', pin[1] - '0', gpioModePushPull, 0);
    }
}

void pinWrite(char pin[], char val[]) {
    if (!(strcmp(val, "hi"))) {
        GPIO_PinOutSet(pin[0] - 'A', pin[1] - '0');
    } else if (!(strcmp(val, "lo"))) {
        GPIO_PinOutClear(pin[0] - 'A', pin[1] - '0');
    } else if (!(strcmp(val, "toggle"))) {
        GPIO_PinOutToggle(pin[0] - 'A', pin[1] - '0');
    }
}

uint32_t pinRead(char pin[]) {
    return GPIO_PinInGet(pin[0] - 'A', pin[1] - '0');
}

void delay(uint32_t usTime) {
    USTIMER_Delay(usTime);
}


void initDHT11(void) {
    pinMode(DHT11Pin, "out");
    pinWrite(DHT11Pin, "lo");
    delay(18000);
    pinWrite(DHT11Pin, "hi");
    delay(20);
    pinMode(DHT11Pin, "in");
}

uint8_t checkResponse(void) {
    uint8_t response = 0;
    delay(40);
    if (!(pinRead(DHT11Pin))) {
        delay(80);
        if (pinRead(DHT11Pin)) {
            response = 1;
        }
        while (pinRead(DHT11Pin));
    }
    return response;
}

uint8_t readBitDHT11(void) {
	uint8_t result;
	while (!(pinRead(DHT11Pin)));
	delay(40);
	if (pinRead(DHT11Pin)) {
		result = 1;
	} else {
		result = 0;
	}
	while(pinRead(DHT11Pin));
	return result;
}

uint8_t readByteDHT11(void) {
	uint8_t result = 0;
	for (int j = 0; j < 8; j++) {
		result <<= 1;
		result |= readBitDHT11();
	}
	return result;
}
void readDataDHT11(void) {
	uint8_t data[5];
	initDHT11();
	checkResponse();
	uint8_t humidInt = readByteDHT11();
	uint8_t humidDec = readByteDHT11();
	uint8_t tempInt = readByteDHT11();
	uint8_t tempDec = readByteDHT11();
	uint8_t sum = data[4];
	app_log("Humidity : %d.%d\n", humidInt, humidDec);
	app_log("Temperature: %d.%d\n", tempInt, tempDec);
}

static volatile bool timer_is_expire = false;
static sl_sleeptimer_timer_handle_t app_timer_handle;

static void app_timer_cb(sl_sleeptimer_timer_handle_t *handle, void *data);

void app_init(void)
{
	USTIMER_Init();
  if (mikroe_mq7_init(IADC0) == SL_STATUS_OK) {
    app_log("Initialize CO sensor successfully\n");

    sl_sleeptimer_start_periodic_timer(&app_timer_handle,
                                       READING_INTERVAL_MSEC,
                                       app_timer_cb,
                                       (void *) NULL,
                                       0,
                                       0);
  }
}

double voltage_adc(uint16_t adc) {
	return ((double) adc) / 1024 * 5;
}

double sensor_resistance(uint16_t adc) {
	return (double) (5 * 10 / voltage_adc(adc) - 10);
}

double ratio(uint16_t adc) {
	return (double) (sensor_resistance(adc) / 15.605);
}

double ppm(uint16_t adc) {
	return (double) (0.35 * pow(ratio(adc), -1.7));
}

/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
  uint16_t data_out;
  readDataDHT11();
  if (timer_is_expire == true) {
    timer_is_expire = false;

    if (mikroe_mq7_read_an_pin_value(&data_out) == SL_STATUS_OK) {
      app_log("PPM value: %.2f\n\n", ppm(data_out));
    } else {
      app_log("Fail to read\n\n");
    }
  }
  delay(1000000);
}

static void app_timer_cb(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void) handle;
  (void) data;

  timer_is_expire = true;
}
