#include "globals.h"
#include "const.h"
#include "utils.h"
#include <Arduino.h>

#include <EEPROM.h>
#include <string.h>
#include <SD.h>
#include <stdint.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <RTClib.h>


uint32_t log_interval = (uint32_t)EEPROM.read(2) * 1000;//(uint32_t)EEPROM.read(2) * 60 * 1000;

void Pressed_Bouton_V() {
  static uint32_t lastInterruptTime_V = 0;
  long currentTime  = millis();

  if (currentTime  - lastInterruptTime_V > DEBOUNCE_DELAY) {
    state_btn_V = digitalRead(PIN_BUTTON_V);
    start_V = millis();
    //Serial.print("Bouton VERT : ");
    //Serial.println(state_btn_V == LOW ? "APPUYÉ" : "RELÂCHÉ");
    lastInterruptTime_V = currentTime ;
  }
}

void Pressed_Bouton_R() {
  static uint32_t lastInterruptTime_R = 0;
  long currentTime  = millis();

  if (currentTime  - lastInterruptTime_R > DEBOUNCE_DELAY) {
    state_btn_R = digitalRead(PIN_BUTTON_R);
    start_R = millis();
    //Serial.print("Bouton ROUGE : ");
    //Serial.println(state_btn_R == LOW ? "APPUYÉ" : "RELÂCHÉ");
    lastInterruptTime_R = currentTime ;
  }
}

void write_ee_default() {
  EEPROM.write(0, eecheckempty);

  uint8_t  TIMEOUT = 30;            // Délai d'attente capteur (s)
  uint8_t  LOG_INTERVAL = 10;       // Intervalle (min)
  uint16_t FILE_MAX_SIZE = 2048;    // Taille max fichier (octets)

  uint8_t  LUMIN = 1;
  uint16_t LUMIN_LOW = 255;
  uint16_t LUMIN_HIGH = 1023;       // valeur par défaut

  uint8_t  TEMP_AIR = 1;
  int16_t  MIN_TEMP_AIR = -10;
  int16_t  MAX_TEMP_AIR = 60;

  uint8_t  HYGR = 1;
  int16_t  HYGR_MINT = 0;
  int16_t  HYGR_MAXT = 50;

  uint8_t  PRESSURE = 1;
  uint16_t PRESSURE_MIN = 850;
  uint16_t PRESSURE_MAX = 1080;

  uint8_t addr = 1;

  EEPROM.write(addr++, TIMEOUT);
  EEPROM.write(addr++, LOG_INTERVAL);

  EEPROM.write(addr++, (FILE_MAX_SIZE >> 0) & 0xFF);
  EEPROM.write(addr++, (FILE_MAX_SIZE >> 8) & 0xFF);

  EEPROM.write(addr++, LUMIN);
  EEPROM.write(addr++, (LUMIN_LOW >> 0) & 0xFF);
  EEPROM.write(addr++, (LUMIN_LOW >> 8) & 0xFF);
  EEPROM.write(addr++, (LUMIN_HIGH >> 0) & 0xFF);
  EEPROM.write(addr++, (LUMIN_HIGH >> 8) & 0xFF);

  EEPROM.write(addr++, TEMP_AIR);
  EEPROM.write(addr++, (MIN_TEMP_AIR >> 0) & 0xFF);
  EEPROM.write(addr++, (MIN_TEMP_AIR >> 8) & 0xFF);
  EEPROM.write(addr++, (MAX_TEMP_AIR >> 0) & 0xFF);
  EEPROM.write(addr++, (MAX_TEMP_AIR >> 8) & 0xFF);

  EEPROM.write(addr++, HYGR);
  EEPROM.write(addr++, (HYGR_MINT >> 0) & 0xFF);
  EEPROM.write(addr++, (HYGR_MINT >> 8) & 0xFF);
  EEPROM.write(addr++, (HYGR_MAXT >> 0) & 0xFF);
  EEPROM.write(addr++, (HYGR_MAXT >> 8) & 0xFF);

  EEPROM.write(addr++, PRESSURE);
  EEPROM.write(addr++, (PRESSURE_MIN >> 0) & 0xFF);
  EEPROM.write(addr++, (PRESSURE_MIN >> 8) & 0xFF);
  EEPROM.write(addr++, (PRESSURE_MAX >> 0) & 0xFF);
  EEPROM.write(addr++, (PRESSURE_MAX >> 8) & 0xFF);

  Serial.print("EEPROM writing complete. Last address = ");
  Serial.println(addr - 1);
}

void write_sd(char *filename, Capteur *p) {
    /* Cette fonction écrit les données mesurées stockées
     * dans une variable de type Capteur pointée par le 
     * pointeur p dans un fichier de nom filename, qui est
     * ensuite stocké dans la carte SD. Elle ne retourne rien.
     */
    if (p == NULL) {
        ledflag = 6;
        return;
    }
    if (!SD.begin(4)) {
        ledflag = 6;
        return;
    }

    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
        ledflag = 6;
        return;
    }

    if (file.size() > file_max_size) {
        file_index++;
        reset_file_name();
    }

    file.print(p->date->annee);
    file.print("-");
    file.print(p->date->mois);
    file.print("-");
    file.print(p->date->jour);
    file.print("_");
    file.print(p->date->heure);
    file.print(":");
    file.print(p->date->minute);
    file.print(":");
    file.print(p->date->seconde);
    file.print(";");
    // ecriture mesures
    for (uint8_t i = 0; i < N_CAPTEUR; i++) {
        file.print(p->mesure[i], 2); // 2 = décimales, mettre en macro ??
        file.print(";");
    }
    file.print("e:");
    file.print(p->erreur);
    file.print("\r\n");
    file.close();
    Serial.println("Saved!");
    real_size += 60;
    
}

void mesure(Capteur *p) {
    /* Cette fonction prend en paramètre un pointeur vers
     * une structure de type Mesure. Elle permet de
     * récupérer les mesures des capteurs et de les stocker
     * dans une variable globale de type struct.
     * Elle ne retourne rien. */

    temp = millis(); 

    if ((temp - start_mesure) > log_interval) { 
        //Serial.println(log_interval);
        int raw_light = analogRead(PIN_LIGHT_SENSOR);
        p->mesure[0] = (float)map(raw_light, 0, 1023, 0, 100);

        fill_rtc(&t);

        if (bme.begin(ADDR_BME_SENSOR)) {
            p->mesure[1] = bme.readTemperature();
            p->mesure[2] = bme.readPressure() / 100.0F;
            p->mesure[3] = bme.readAltitude(SEALEVELPRESSURE_HPA);
            p->mesure[4] = bme.readHumidity();
        } else {
            p->erreur = 1;
        }


        if (mode == 0 || mode == 3){ 
            write_sd(filename, p);
        }
        if (mode == 2){
            Serial.print("heure actuelle : ");
            Serial.print(c.date->heure + 1);
            Serial.print(":");
            Serial.print(c.date->minute);
            Serial.print(":");
            Serial.println(c.date->seconde);

            for (uint8_t i = 0; i < N_CAPTEUR; i++) {
                Serial.print("Capteur n. ");
                Serial.print(i + 1);
                Serial.print(" : ");
                Serial.println(c.mesure[i]);
            }
        }
        
        if (c.erreur) {
            ledflag = 3;
        }
        start_mesure = millis();
    }
}

void fill_rtc(Temps *t) {
    DateTime now = rtc.now();

    t->annee = now.year() - 2000;
    t->mois = now.month();
    t->jour = now.day();
    t->heure = now.hour();
    t->minute = now.minute();
    t->seconde = now.second();
}

void reset_file_name() {
    fill_rtc(&t);
    uint8_t annee = t.annee;
    snprintf(filename, sizeof(filename), "%02u%02u%02u_%u.log", annee, t.mois, t.jour, file_index);
    //Serial.print("filename: ");
    //Serial.println(filename);

}


uint32_t start_blink = millis();
bool state = false;

void blink(uint8_t color[3], uint8_t color2[3], uint8_t temp1, uint8_t temp2){
    leds.setColorRGB(0, color[0], color[1], color[2]);
    delay(temp1);
    leds.setColorRGB(0, color2[0], color2[1], color2[2]);
    delay(temp2);
}


uint8_t rouge[3] = {255, 0, 0};
uint8_t bleu[3] = {0, 0, 255};
uint8_t vert[3] = {0, 255, 0};
uint8_t jaune[3] = {255, 255, 0};
uint8_t blanc[3] = {255, 255, 255};

void LED_err() {
    /* Cette fonction fait clignoter la LED selon l'erreur.
     * Ne retourne rien. */
    //Serial.print("Error: ");
    //Serial.println(ledflag);
    switch (ledflag) {
        case 0:
            return;
        case 1:
            // LED -> erreur horloge
            blink(rouge, bleu, 1, 1);
            return;
        case 2:
            // LED -> erreur GPS
            blink(rouge, jaune, 1, 1);
            return;
        case 3:
            // LED -> erreur capteur
            blink(rouge, vert, 1, 1);
            return;
        case 4:
            // LED -> données capteur incohérentes
            blink(rouge, vert, 1, 2);
            return;
        case 5:
            // LED -> carte SD pleine
            blink(rouge, blanc, 1, 1);
            return;
        case 6:
            // LED -> erreur accès / écriture carte SD
            blink(rouge, blanc, 1, 2);
            Serial.println("Cartsd pas trouvé");
            return;
    }
}