/**
  ******************************************************************************
  * @file    bsp_i2c.c 
  * @author  Xiao Yang 260384793@qq.com
  * @version V1.0.0
  * @date    2021-10-06
  * @brief   
  ******************************************************************************
  */

/* Include -------------------------------------------------------------------*/
#include "bsp_i2c.h"
#include "sf_i2c.h"

/* Private function ----------------------------------------------------------*/
__STATIC_INLINE void i2c0_sda_pin_out_low(void);
__STATIC_INLINE void i2c0_sda_pin_out_high(void);
__STATIC_INLINE void i2c0_scl_pin_out_low(void);
__STATIC_INLINE void i2c0_scl_pin_out_high(void);
__STATIC_INLINE uint8_t i2c0_sda_pin_read_level(void);
__STATIC_INLINE void i2c0_sda_pin_dir_input(void);
__STATIC_INLINE void i2c0_sda_pin_dir_output(void);

/* Private variables ---------------------------------------------------------*/
// 定义iic1驱动对象
static i2c_dev i2c0_dev = {
    .name                    = I2C0_NAME,
    .speed                   = 40, /*! speed:120Hz */
    .port.sda_pin_out_low    = i2c0_sda_pin_out_low,
    .port.sda_pin_out_high   = i2c0_sda_pin_out_high,
    .port.scl_pin_out_low    = i2c0_scl_pin_out_low,
    .port.scl_pin_out_high   = i2c0_scl_pin_out_high,
    .port.sda_pin_read_level = i2c0_sda_pin_read_level,
    .port.sda_pin_dir_input  = i2c0_sda_pin_dir_input,
    .port.sda_pin_dir_output = i2c0_sda_pin_dir_output,
};

/*! Set i2c sda pin low level */
__STATIC_INLINE void i2c0_sda_pin_out_low(void)
{
    GPIO_ResetBits(I2C0_PORT, I2C0_SDA_PIN);
}

/*! Set i2c sda pin high level */
__STATIC_INLINE void i2c0_sda_pin_out_high(void)
{
    GPIO_SetBits(I2C0_PORT, I2C0_SDA_PIN);
}

/*! Set i2c scl pin low level */
__STATIC_INLINE void i2c0_scl_pin_out_low(void)
{
    GPIO_ResetBits(I2C0_PORT, I2C0_SCL_PIN);
}

/*! Set i2c scl pin high level */
__STATIC_INLINE void i2c0_scl_pin_out_high(void)
{
    GPIO_SetBits(I2C0_PORT, I2C0_SCL_PIN);
}

/*! Read i2c sda pin level */
__STATIC_INLINE uint8_t i2c0_sda_pin_read_level(void)
{
    return GPIO_ReadInputDataBit(I2C0_PORT, I2C0_SDA_PIN);
}

/*! Switch i2c sda pin dir input */
__STATIC_INLINE void i2c0_sda_pin_dir_input(void)
{
    I2C0_PORT->MODER &=0XFFFFFFFC;
}

/*! Switch i2c sda pin dir output */
__STATIC_INLINE void i2c0_sda_pin_dir_output(void)
{
    I2C0_PORT->MODER &=0XFFFFFFFC;
    I2C0_PORT->MODER |= 0x01;
}

/**
 * @brief  Initialization i2c0 physical interface
 * @param  none
 * @return none
 */
static void i2c0_phy_init(void)
{
    // Config pin output direction
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Pin   = I2C0_SDA_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(I2C0_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Pin   = I2C0_SCL_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(I2C0_PORT, &GPIO_InitStruct);
}

/**
 * @brief  Initialization board i2c0 interface
 * @param  none
 * @return none
 */
void bsp_i2c_init(void)
{
    /*! i2c physical layer initialization */
    i2c0_phy_init();

    /*! i2c software layer initialization */
    i2c_init(&i2c0_dev);
}
