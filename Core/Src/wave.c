#include "main.h"
#include "dso.h"
#include "wave.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#define WAVE_COLOR YELLOW
// All kinds of variables, you'll see what these do in scope.c
extern uint16_t adcBuf[BUFFER_LEN];
extern int atten;
extern float vdiv;
extern float trigVoltage;
extern uint8_t trig, trigged;
extern int trigPoint;

extern float tdiv;
extern uint32_t sampRate;
extern float sampPer;

extern float maxVoltage, minVoltage;
extern float measuredFreq, sigPer;

extern float offsetVoltage;

uint8_t topClip, bottomClip; // Whether or not we're clipping through the graticule
// Convert ADC sample to voltage at ADC input
float adcToVoltage(uint16_t samp)
{
    return (3.3 * samp) / 4096.0;
}
void findTrigger(uint16_t adcBuf[])
{
    int trigLevel = (4096.0 * (trigVoltage / (2.0 * atten) + offsetVoltage)) / 3.3; // ADC level at which we should trigger
    int trigPoint2;                                                                 // another trigger point, this will help us determine the period of the signal

    trigPoint = 0;
    trigged = 0;
    measuredFreq = 0;

    // The trigged variable will be 0 if we're not triggering, 1 if we only found 1 trigger point and 2 if we have at least two trigger points

    for (int i = 1; i < BUFFER_LEN / 2 && trigged != 2; i++) // we're looking for trigger points in the first half of the buffer
        if ((trig == RISING && adcBuf[i] >= trigLevel && adcBuf[i - 1] < trigLevel) || (trig == FALLING && adcBuf[i] <= trigLevel && adcBuf[i - 1] > trigLevel))
        {
            if (!trigged) // Looking for the first trigger point
            {
                trigPoint = i;
                trigged = 1;
            }
            else // Looking for the second one
            {
                trigPoint2 = i;
                trigged = 2;
            }
        }

    if (trigged == 2) // If we found at least two trigger points
    {
        sigPer = sampPer * (trigPoint2 - trigPoint); // we compute the period of the signal in uS
        measuredFreq = 1000000.0 / sigPer;           // and then we convert it into frequency, in Hz
    }
}

void drawGraticule(uint16_t divx, uint16_t divy, uint16_t pix)
{
    uint16_t wit = divx * pix;
    uint16_t hei = divy * pix;

    for (int i = 0; i <= wit; i += pix)
    ILI9341_DrawVLine(i, 0, hei, WHITE);

    for (int i = 0; i <= hei; i += pix)
    ILI9341_DrawHLine(0, i, wit, WHITE);
}

// Draw the waveform trace on the screen
void drawTrace(uint16_t buf[], uint16_t trigPoint, uint16_t col)
{

    maxVoltage = LOWER_VOLTAGE;
    minVoltage = UPPER_VOLTAGE;

    for (int i = 0; i <= BUFFER_LEN / 2; i++)
    {
        // If we're looping through the buffer, let's compute the minimum and maximum voltage values while we're at it
        float voltage1 = atten * adcToVoltage(buf[i + trigPoint]);
        float voltage2 = atten * adcToVoltage(buf[i + trigPoint + 1]);
        if (voltage2 > maxVoltage)
            maxVoltage = voltage2;
        if (voltage2 < minVoltage)
            minVoltage = voltage2;

        // Draw lines between sample points
        topClip = 0;
        bottomClip = 0;
        int16_t y1 = (PIXDIV * YDIV / 2 - 1) - (voltage1 * PIXDIV / vdiv);
        int16_t y2 = (PIXDIV * YDIV / 2 - 1) - (voltage2 * PIXDIV / vdiv);
        if (y1 > YDIV * PIXDIV)
        {
            y1 = YDIV * PIXDIV;
            bottomClip = 1;
        }
        if (y2 > YDIV * PIXDIV)
        {
            y2 = YDIV * PIXDIV;
            bottomClip = 1;
        }
        if (y1 < 0)
        {
            y1 = 0;
            topClip = 1;
        }
        if (y2 < 0)
        {
            y2 = 0;
            topClip = 1;
        }
        ILI9341_DrawLine(i, y1, i + 1, y2, col);
    }
}

// Draw the trace and graticule on the screen
void traceScreen()
{
    drawGraticule(XDIV, YDIV, PIXDIV); // Draw the graticule
    drawTrace(adcBuf, trigPoint, WAVE_COLOR);
}


