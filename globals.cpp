#include "globals.h"
#include <stdint.h>
#include <Adafruit_BME280.h>
#include <RTClib.h>
#include <ChainableLED.h>

Adafruit_BME280 bme;
RTC_DS3231 rtc;

ChainableLED leds(PIN1_LED, PIN2_LED, NUM_LEDS);

Temps t;
Capteur c;

uint8_t intervalflag = 0; // = 1 à chaque appel au mode eco
uint8_t ledflag = 0; // code selon l'erreur

uint16_t file_max_size;
uint8_t real_size;

char filename[32];
uint8_t file_index = 0;

uint8_t mode = 0;

uint8_t state_btn_V = HIGH; // variable d'état du bouton vert, pressé ou non
uint8_t state_btn_R = HIGH; // variable d'état du bouton rouge, pressé ou non

uint32_t start_V = 0;
uint32_t start_R = 0;

uint32_t temp = 0; // variable du temps instantané

uint32_t start_mesure;