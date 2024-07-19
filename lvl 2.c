#include "em_gpio.h"
#include "app_log.h"
#include "sl_sleeptimer.h"
#include "string.h"

void delay(int time) { // delay time for millisecond
  sl_sleeptimer_delay_millisecond(time);
}

void pinInit(char pin[], char mode[]) { // initialize pin
  if (strcmp(mode, "output") == 0) {
      GPIO_PinModeSet(pin[0] - 'A', pin[1] - '0', gpioModePushPull, 0);
  } else if (strcmp(mode, "input") == 0) {
      GPIO_PinModeSet(pin[0] - 'A', pin[1] - '0', gpioModeInput, 0);
  }
}

void pinModify(char pin[], char action[]) { // modify pin actions
  if (strcmp(action, "on") == 0) {
      GPIO_PinOutSet(pin[0] - 'A', pin[1] - '0');
  } else if (strcmp(action, "toggle") == 0) {
      GPIO_PinOutToggle(pin[0] - 'A', pin[1] - '0');
  } else if (strcmp(action, "off") == 0) {
      GPIO_PinOutClear(pin[0] - 'A', pin[1] - '0');
  }
}

void pinBlink(char pin[], int interval) { // pin blink behaviour for duration (millisecond)
  pinModify(pin, "toggle");
  delay(interval);
  pinModify(pin, "toggle");
}

void app_init(void)
{
  pinInit("D3", "output");
  pinInit("B1", "output");
  pinInit("B3", "output");
  pinInit("C3", "output");
}

/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
  pinBlink("D3", 1000);
  pinBlink("B1", 1000);
  pinBlink("B3", 1000);
  pinBlink("C3", 1000);
}
