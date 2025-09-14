#include "main.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "dso.h"
#include "wave.h"
#include "ui.h"
#include "init.h"
extern ADC_HandleTypeDef hadc;
extern DMA_HandleTypeDef hdma_adc;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim3;

uint16_t adcBuf[BUFFER_LEN];             // this is where we'll store waveform data
volatile uint8_t finishedConversion = 0; // this lets us know when we're done capturing data

int atten = 1;  // Attenuation
float vdiv = 1; // Volts per division

uint8_t trigged;       // whether or not we're triggered
int trigPoint;         // triggering point
float trigVoltage = 0; // Trigger level
uint8_t trig = RISING; // Trigger slope

float tdiv = 300;   // uS per division
uint32_t sampRate; // Sample rate
float sampPer;     // Sample period in uS (how long it takes to measure one sample)

float maxVoltage, minVoltage; // Voltage measurements
float measuredFreq, sigPer;   // Time measurements

float offsetVoltage = 1.1; // Reference voltage of the the analog frontend

void DSO_Init(void)
{
	ILI9341_Init();
	ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
    createFramebuf(); // Create the framebuffer for the LCD
    clearDisplay();
    // Set the sampling rate
    sampRate = (16000 * 1000) / tdiv;
    sampPer = tdiv / 16.0;
    SetTimerFreq(sampRate);
}

// This function acquires one buffer worth of data
void Sample()
{
    HAL_TIM_Base_Start(&htim3);
    HAL_ADC_Start_DMA(&hadc, (uint32_t *)adcBuf, BUFFER_LEN);
    while (!finishedConversion)
        ;
    HAL_TIM_Base_Stop(&htim3);
    finishedConversion = 0;
}

// This is the main loop of the app
void DSO_Loop()
{
	while(1)
	{
	    // Acquire one buffer
	    Sample();

	    // Find the trigger point
	    findTrigger(adcBuf);
	    if (trigged)
	        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1); // light the on-board LED up if triggered

	    // Run the UI
	    ui();
	    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
	}

}
// This sets the sampling rate
void SetTimerFreq(uint32_t freq)
{
    uint16_t arr = (SYSCLK_FREQ / ((CLOCKTIM_PRESC + 1) * freq)) - 1;
    htim3.Instance->ARR = arr;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    finishedConversion = 1;
}
