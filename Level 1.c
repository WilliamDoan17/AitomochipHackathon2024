#include <app_log.h>
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

void app_init(void) {
    USTIMER_Init();
    pinMode("D3", "out");
}

void app_process_action(void) {
    pinWrite("D3", "hi");
    delay(1000000);
}
