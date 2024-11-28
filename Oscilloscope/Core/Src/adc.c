#include "stm32f4xx.h"

#define RCC_GPIOD_EN (0b01<<3)
#define RCC_TIM2_ENR (0b01)
#define GPIOD_PORT14_OUTPUT (0b01 << 28)
#define PORT14 (1<<14)
void init_adc(void)
{
	// Setting Pin 14 to OUTPUT
	RCC->AHB1ENR |= RCC_GPIOD_EN;
	GPIOD->MODER |= GPIOD_PORT14_OUTPUT;

	  GPIOD->BSRR |= PORT14;

}
