#ifndef CONST_H
#define CONST_H

#include <stdint.h>

constexpr uint8_t eecheckempty = 0xFF;
constexpr uint32_t DEBOUNCE_DELAY = 100;

#define PIN_BUTTON_V 2
#define PIN_BUTTON_R 3
#define NUM_LEDS 1
#define PIN1_LED 4
#define PIN2_LED 5
#define PIN_LIGHT_SENSOR A3
#define N_CAPTEUR 5
#define ADDR_BME_SENSOR 0x76
#define SEALEVELPRESSURE_HPA (1013.25)

#endif