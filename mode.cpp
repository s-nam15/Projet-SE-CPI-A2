#include "globals.h"
#include "utils.h"
#include "const.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

void mode_standard(Capteur *p) {
    /* Cette fonction permet d'arrêter les procédures 
     * engagées dans les autres fonctions mode_*.
     * + faire mesure et écrire SD.
     * Ne retourne rien.*/
    if (intervalflag) {
        uint8_t val;
        val = EEPROM.read(2);
        val = val >> 1; // shift droit (divise par 2)
        EEPROM.write(2, val);
        intervalflag = 0;
    }
    mesure(p);
}

void mode_config() {
    /* Cette fonction prend une entrée de l'utilisateur
     * sur l'interface série, lui permettant de modifier
     * les valeurs des paramètres enregistrés dans l’EEPROM.
     * Ne retourne rien. */
    char buffer[32];
    if (Serial.available() > 0) {
        uint8_t len = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
        buffer[len] = '\0';
        Serial.println(buffer);
        char *key = NULL;
        char *value = NULL;
        key = strtok(buffer, "=");
        value = strtok(NULL, "=");
        if (key && value) {
            if (!strcmp(key, "TIMEOUT")) {
                uint8_t val = atoi(value);
                EEPROM.write(1, val);
            } else if (strcmp(key, "LOG_INTERVAL") == 0) {
                char *endptr;
                long val = strtoul(value, &endptr, 10);
                if (*endptr == '\0' && val >= 0 && val <= 255) {
                    EEPROM.update(2, (uint8_t)val);
                }
            } else if (!strcmp(key, "FILE_MAX_SIZE")) {
                uint16_t val = (uint16_t)atoi(value);
                EEPROM.write(3, (val >> 0) & 0xFF);
                EEPROM.write(4, (val >> 8) & 0xFF);
            }
        } else if (key && !value) {
            if (!strcmp(key, "RESET")) {
                write_ee_default();
            } else if (!strcmp(key, "VERSION")) {
                Serial.println("pas compris la doc sur 'version'");
            }
        } else {
            Serial.println("Erreur format invalide");
        }
    }
}

void mode_maintenance() {
    /* Cette fonction permet de consulter les données 
     * en temps réel dans le terminal. L'écriture
     * des données dans la carte SD est arrêtée */
    mesure(&c);
    
}

void mode_eco() {
    /* Cette fonction multiplie le temps entre 2 mesures
     * par 2. */
    uint8_t val;
    val = EEPROM.read(2);
    val = val << 1; // shift gauche (multiplication par 2)
    EEPROM.write(2, val);
    intervalflag = 1;

    mesure(&c);
}