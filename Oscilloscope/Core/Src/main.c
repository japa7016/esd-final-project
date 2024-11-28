#include "main.h"
#include "gpio.h"
#include "i2c.h"

#include "ssd1306.h"
#include "ssd1306_tests.h"


int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  ssd1306_Init();
  ssd1306_TestFonts3();
  while (1)
  {
  }
}







