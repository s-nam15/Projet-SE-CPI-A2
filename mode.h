#ifndef MODE_H
#define MODE_H

#include "globals.h"
#include "const.h"

#include <stdint.h>

void mode_standard(Capteur *p);
void mode_config();
void mode_maintenance();
void mode_eco();

#endif