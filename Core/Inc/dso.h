#ifndef DSO_H
#define DSO_H

#include <stdint.h>
#define PIXDIV 16
#define XDIV 15
#define YDIV 10

#define CLOCKTIM_PRESC 0
#define SYSCLK_FREQ 48000000
#define BUFFER_LEN (2 * PIXDIV * XDIV)

#define UPPER_VOLTAGE (atten * 3.3)
#define LOWER_VOLTAGE (atten * -3.3)

#define RISING 1
#define FALLING 0

void DSO_Init();
void Sample();
void DSO_Loop();
void SetTimerFreq(uint32_t freq);

#endif
