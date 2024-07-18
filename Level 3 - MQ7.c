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
#include "math.h";
#include "mikroe_mq7.h"

#define READING_INTERVAL_MSEC 3000

static volatile bool timer_is_expire = false;
static sl_sleeptimer_timer_handle_t app_timer_handle;

static void app_timer_cb(sl_sleeptimer_timer_handle_t *handle, void *data);

void app_init(void)
{
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

  if (timer_is_expire == true) {
    timer_is_expire = false;

    if (mikroe_mq7_read_an_pin_value(&data_out) == SL_STATUS_OK) {
      app_log("PPM value: %.2f\n", ppm(data_out));
    } else {
      app_log("Fail to read\n");
    }
  }
}

static void app_timer_cb(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void) handle;
  (void) data;

  timer_is_expire = true;
}
