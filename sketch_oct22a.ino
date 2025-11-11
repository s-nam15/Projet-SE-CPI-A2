/* Bibliothèques utilisées */
#include "globals.h"
#include "const.h"
#include "utils.h"
#include "mode.h"
#include <EEPROM.h>

#include <ChainableLED.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <RTClib.h>

uint32_t last_action_time;

/* Fonction setup */
void setup() {
    Serial.begin(9600);
    Wire.begin();

    c.date = &t;

    
    // Récupération de l'heure du PC
    DateTime heurePC(F(__DATE__), F(__TIME__));
    rtc.adjust(heurePC);


    //uint8_t flag = EEPROM.read(0);
    //if (flag != eecheckempty) {
        write_ee_default();
        file_max_size = (EEPROM.read(4) << 8) | EEPROM.read(3);
    //}

    /* Initialisation des PIN Bouton */
    pinMode(PIN_BUTTON_V, INPUT_PULLUP);
    pinMode(PIN_BUTTON_R, INPUT_PULLUP);
    /* Initialisation des PIN LED */
    pinMode(PIN1_LED, OUTPUT);
    pinMode(PIN2_LED, OUTPUT);
    
    /* Initialisation des interruptions */
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_V), Pressed_Bouton_V, CHANGE); // Interruption bouton Vert
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_R), Pressed_Bouton_R, CHANGE); // Interruption bouton Rouge
   
    
    delay(50);  

    bool boutonR_state_1 = digitalRead(PIN_BUTTON_R);
    delay(20);
    bool boutonR_state_2 = digitalRead(PIN_BUTTON_R);

    if (boutonR_state_1 == boutonR_state_2 && boutonR_state_1 == LOW) {
        mode = 1;  // Bouton appuyé
        // Réinitialiser l'état après changement de mode
        state_btn_R = HIGH;
        start_R = millis();
    } else {
        mode = 0;  // Bouton non appuyé
    }

    if (!bme.begin(ADDR_BME_SENSOR)) {
        Serial.println("Pas pu trouver de capteur BME280");
        ledflag = 3;
    } else {
        Serial.println("Capteur BME280 trouvé avec succès");
    }

    
    if (!rtc.begin()) {
        Serial.println("RTC non détéctée");
        ledflag = 1;
    }
    
    last_action_time = millis();

    reset_file_name();

    start_mesure = millis();
}

void loop(){
    unsigned long now = millis();
    
  /*  Serial.print("Bouton VERT : ");
    Serial.print(state_btn_V == LOW ? "APPUYÉ" : "RELÂCHÉ");
    Serial.print(" | Bouton ROUGE : ");
    Serial.println(state_btn_R == LOW ? "APPUYÉ" : "RELÂCHÉ");*/

    switch (mode) {
        case 0: // Standard
            leds.setColorRGB(0, 0, 255, 0);
            mode_standard(&c);
            if(state_btn_V == LOW && millis() - start_V > 2000){
                mode = 3; // Passage config via bouton vert
                state_btn_V = HIGH;
                start_V = millis();
            }
            if(state_btn_R == LOW && millis() - start_R > 2000){
                mode = 2; // Maintenance
                state_btn_R = HIGH;
                start_R = millis();
            }
            break;
        case 1: // Config
            leds.setColorRGB(0, 255, 80, 0); 
            mode_config(); 
            if(state_btn_R == LOW){ 
                temp = millis(); 
                if((temp - start_R) > 2000){ 
                    // Utilisez start_R pour le bouton rouge 
                    //Serial.println("Changement de mode par bouton Rouge\n"); 
                    mode = 0; // Mode Standard 
                    // Réinitialiser l'état après changement de mode 
                    state_btn_R = HIGH; 
                    start_R = millis(); 
                    } 
                } 
                break;
            case 2: // maintenance
                leds.setColorRGB(0, 255, 165, 0);
                mode_maintenance(); 
                if(state_btn_R == LOW){
                    temp = millis(); 
                    if((temp - start_R) > 2000){
                        // Utilisez start_R pour le bouton rouge
                        //Serial.println("Changement de mode par bouton Rouge\n"); 
                        mode = 0; // Mode Standard 
                        // Réinitialiser l'état après changement de mode 
                        state_btn_R = HIGH; 
                        start_R = millis(); 
                    } 
                } 
                break;
        case 3: // Eco
            leds.setColorRGB(0, 0, 0, 255);
            mode_eco();
            if(state_btn_R == LOW && millis() - start_R > 2000){
                mode = 0; // Retour Standard
                state_btn_R = HIGH;
                start_R = millis();
            }
            /*if(state_btn_V == LOW && millis() - start_V > 2000){
                mode = 1; // Retour Standard
                state_btn_V = HIGH;
                start_V = millis();
            }*/
            break;
    }
    LED_err();
}