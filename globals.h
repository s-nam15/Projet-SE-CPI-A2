#ifndef GLOBALS_H
#define GLOBALS_H

#include "const.h"
#include <stdint.h>
#include <Adafruit_BME280.h>
#include <RTClib.h>
#include <ChainableLED.h>

extern Adafruit_BME280 bme;
extern RTC_DS3231 rtc;
extern ChainableLED leds;

extern uint8_t intervalflag;
extern uint8_t ledflag;
/* 0 - pas d'erreur
* 1 - err horloge
* 2 - err GPS
* 3 - err capteur
* 4 - données capteur incohérentes
* 5 - carte SD pleine
* 6 - err accès / écriture carte SD
*/
extern char filename[32];
extern uint8_t file_index;

extern uint16_t file_max_size;
extern uint8_t real_size;

extern uint8_t mode;
/* Mode :
 * 0 - Standard
 * 1 - Configuration
 * 2 - Maintenance
 * 3 - Economique
 */

extern uint8_t state_btn_V; // variable d'état du bouton vert, pressé ou non
extern uint8_t state_btn_R; // variable d'état du bouton rouge, pressé ou non

extern uint32_t start_V;
extern uint32_t start_R;

extern uint32_t temp; // variable du temps instantané
extern uint32_t start_mesure;

typedef struct {
    uint8_t annee;
    uint8_t mois;
    uint8_t jour;
    uint8_t heure;
    uint8_t minute;
    uint8_t seconde;
} Temps;
extern Temps t;

typedef struct {
    Temps *date;
    float mesure[N_CAPTEUR];
    uint8_t erreur; // 0 ou 1
} Capteur;
extern Capteur c;

#endif