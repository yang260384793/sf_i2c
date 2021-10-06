/**
  ******************************************************************************
  * @file    bsp_i2c.h 
  * @author  Xiao Yang 260384793@qq.com
  * @version V1.0.0
  * @date    2021-10-06
  * @brief   
  ******************************************************************************
  */

#ifndef __BSP_I2C_H
#define __BSP_I2C_H

/* Include -------------------------------------------------------------------*/
#include "stm32f4xx.h"

/*******************************************************************************
 * i2c0 physical interface
 ******************************************************************************/
#define I2C0_PORT_CLOCK     RCC_AHB1Periph_GPIOB
#define I2C0_PORT           GPIOB
#define I2C0_SDA_PIN        GPIO_Pin_0
#define I2C0_SCL_PIN        GPIO_Pin_1

#define I2C0_NAME           (char*)"I2C0"

/* Exported functions --------------------------------------------------------*/
void bsp_i2c_init(void);

#endif
