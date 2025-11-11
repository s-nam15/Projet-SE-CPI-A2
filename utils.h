#ifndef UTILS_H
#define UTILS_H

#include "globals.h"

#include <stdint.h>

void Pressed_Bouton_V();
void Pressed_Bouton_R();
void write_ee_default();
void write_sd(char *filename, Capteur *p);
void mesure(Capteur *p);
void fill_rtc(Temps *t);
void LED_err();
void reset_file_name();

#endif