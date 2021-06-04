#ifndef POWER_H
#define POWER_H

#include "stdint.h"

void LowPowerConfig(void);
void PreIntoStopMode(void);
void IntoStopModeWithRTC(uint32_t sec);
void WakeFromStopMode(void);

#endif