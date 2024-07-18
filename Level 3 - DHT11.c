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
	app_log("Temperature: %d.%d\n\n", tempInt, tempDec);
}

void app_init(void) {
  USTIMER_Init();
}
void app_process_action (void) {
	readDataDHT11();
    delay(1000000);
}
