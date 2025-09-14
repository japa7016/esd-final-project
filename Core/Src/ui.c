#include "dso.h"
#include "main.h"
#include "wave.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include <stdio.h>
#define MENUPOS 254

extern uint16_t adcBuf[BUFFER_LEN];
extern int atten;
extern float vdiv;
extern float trigVoltage;
extern uint8_t trig;
extern uint8_t trigged;
extern int trigPoint;

extern float tdiv;
extern uint32_t sampRate;
extern float sampPer;

extern float maxVoltage, minVoltage;
extern float measuredFreq, sigPer;

volatile uint8_t outputFlag = 0;

uint8_t autocalFlag = 0;
extern float offsetVoltage;
volatile uint8_t sel = 0;
uint8_t fast = 1;

// A little startup splash screen
// Vertical autocalibration
void ui(void)
{
	clearDisplay();
    traceScreen();
    sideInfo();
    settingsBar();
}
// This function displays voltage info in the side menu
void sideInfo(void)
{
    char st[15];

    // Display Minimum Voltage
    sprintf(st, "%.1f", minVoltage);
    ILI9341_DrawText("Min:", FONT3, MENUPOS, 10, BLACK, WHITE);  // Adjusted y-coordinate for FONT4
    ILI9341_DrawText(st, FONT3, MENUPOS, 30, WHITE, BLACK);      // Spaced further down

    // Display Maximum Voltage
    sprintf(st, "%.1f", maxVoltage);
    ILI9341_DrawText("Max:", FONT3, MENUPOS, 50, BLACK, WHITE); // Further spacing for FONT4 size
    ILI9341_DrawText(st, FONT3, MENUPOS, 70, WHITE, BLACK);

    // Display Peak-to-Peak Voltage
    sprintf(st, "%.1fV", maxVoltage - minVoltage);
    ILI9341_DrawText("Vpp:", FONT3, MENUPOS, 90, BLACK, WHITE);
    ILI9341_DrawText(st, FONT3, MENUPOS, 110, WHITE, BLACK);

    // Display Frequency
    ILI9341_DrawText("Freq", FONT3, MENUPOS, 130, BLACK, WHITE);
    if (measuredFreq >= 1000)
    {
        if (measuredFreq >= 100000)
        {
            sprintf(st, "%d", (int)(measuredFreq / 1000));
        }
        else
        {
            sprintf(st, "%.1f", measuredFreq / 1000);
        }
        ILI9341_DrawText(st, FONT3, MENUPOS, 150, WHITE, BLACK);
        ILI9341_DrawText("kHz", FONT3, MENUPOS, 170, WHITE, BLACK);
    }
    else
    {
        sprintf(st, "%d", (int)measuredFreq);
        ILI9341_DrawText(st, FONT3, MENUPOS, 150, WHITE, BLACK);
        ILI9341_DrawText("Hz", FONT3, MENUPOS, 170, WHITE, BLACK);
    }

    if (trigged)
    {
        ILI9341_DrawText("Trig", FONT3, MENUPOS, 190, GREEN, BLACK);
    }   // Print info on the side
}

// This function adjusts the settings
void settingsBar()
{
	   extern uint8_t topClip, bottomClip;

	    char st[10];

	    // Print top row with adjusted positions
	    if (topClip || bottomClip)
	        ILI9341_DrawText("Vdiv", FONT3, 10, 170, RED, BLACK);  // Adjusted for FONT4
	    else
	        ILI9341_DrawText("Vdiv", FONT3, 10, 170, WHITE, BLACK);

	    ILI9341_DrawText("Trig", FONT3, 50, 170, WHITE, BLACK);
	    ILI9341_DrawText("Slope", FONT3, 90, 170, WHITE, BLACK);
	    ILI9341_DrawText("Atten", FONT3, 140, 170, WHITE, BLACK);
	    ILI9341_DrawText(tdiv < 100 ? "us/d" : "ms/d", FONT3, 190, 170, WHITE, BLACK);

	    // Print bottom row values
	    if (sel == 0)
	    {
	    	ILI9341_DrawText("Vdiv", FONT3, 10, 170, BLACK, WHITE);
	        if (topClip || bottomClip)
	            ILI9341_DrawText(st, FONT4, 10, 190, RED, BLACK);
	        else
	            ILI9341_DrawText(st, FONT4, 10, 190, WHITE, BLACK);
	    }
	    sprintf(st, "%.1fV", vdiv);
	    ILI9341_DrawText(st, FONT3, 10, 190, WHITE, BLACK);

	    if (sel == 1)
	    {
	    	ILI9341_DrawText("Trig", FONT3, 50, 170, BLACK, WHITE);
	        ILI9341_DrawHLine(0, (uint16_t)((PIXDIV * YDIV / 2 - 1) - (trigVoltage * PIXDIV / vdiv)), XDIV * PIXDIV, RED);
//	        ILI9341_DrawText("Trig Voltage:", FONT3, 50, 190, WHITE, BLACK);
	    }
	    sprintf(st, "%.1f", trigVoltage);
	    ILI9341_DrawText(st, FONT3, 50, 190, WHITE, BLACK);

	    if (sel == 2)
	    {
	    	ILI9341_DrawText("Slope", FONT3, 90, 170, BLACK, WHITE);
	        ILI9341_DrawText(trig == RISING ? "Rise" : "Fall", FONT3, 90, 190, WHITE, BLACK);
	    }

	    if (sel == 3)
	    {
	    	ILI9341_DrawText("Atten", FONT3, 140, 170, BLACK, WHITE);
	        sprintf(st, "%dx", atten);
	        ILI9341_DrawText(st, FONT3, 140, 190, WHITE, BLACK);
	    }

	    if (sel == 4)
	    {
	    	ILI9341_DrawText(tdiv < 100 ? "us/d" : "ms/d", FONT3, 190, 170, BLACK, WHITE);
	        if (tdiv < 100)
	            sprintf(st, "%d", (int)tdiv);
	        else if (tdiv < 1000)
	            sprintf(st, "0.%d", (int)tdiv / 100);
	        else
	            sprintf(st, "%d", (int)tdiv / 1000);
	        ILI9341_DrawText(st, FONT3, 190, 190, WHITE, BLACK);
	    }
}


void dso_increase(void)
{
    switch (sel)
    {
        case 0: if (vdiv < 9) vdiv += 0.5; break;
        case 1: trigVoltage += 0.1; break;
        case 2: trig = RISING; break;
        case 3: atten = 10; break;
        case 4:
            if (tdiv >= 1000) tdiv += 1000;
            else if (tdiv >= 100) tdiv += 100;
            else if (tdiv >= 10) tdiv += 10;

            sampRate = (PIXDIV * 1000 * 1000) / tdiv;
            sampPer = tdiv / (float)PIXDIV;
            SetTimerFreq(sampRate);
            break;
    }
    HAL_Delay(150);
}

void dso_decrease(void)
{
    switch (sel)
    {
        case 0: if (vdiv > 0.5) vdiv -= 0.5; break;
        case 1: trigVoltage -= 0.1; break;
        case 2: trig = FALLING; break;
        case 3: atten = 1; break;
        case 4:
            if (tdiv > 10)
            {
                if (tdiv > 1000) tdiv -= 1000;
                else if (tdiv > 100) tdiv -= 100;
                else if (tdiv > 10) tdiv -= 10;
            }
            sampRate = (PIXDIV * 1000 * 1000) / tdiv;
            sampPer = tdiv / (float)PIXDIV;
            SetTimerFreq(sampRate);
            break;
    }
    HAL_Delay(150);
}

void dso_select(void)
{
    sel++;
    if (sel > 4) sel = 0;
    HAL_Delay(150);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_13 && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET)
    {
        dso_decrease();
    }
    else if (GPIO_Pin == GPIO_PIN_11 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_RESET)
    {
        dso_select();
    }
    else if (GPIO_Pin == GPIO_PIN_12 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET)
    {
        dso_increase();
    }
}
