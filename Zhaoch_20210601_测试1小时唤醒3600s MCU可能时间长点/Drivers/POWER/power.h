#ifndef POWER_H
#define POWER_H

#include "stdint.h"

void LowPowerConfig(void);
void PreIntoStopMode(void);
void IntoStopModeWithRTC(uint16_t sec);
void WakeFromStopMode(void);

#endif