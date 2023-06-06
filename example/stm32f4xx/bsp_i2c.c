/**
  ******************************************************************************
  * @file    bsp_i2c.c 
  * @author  Xiao Yang 260384793@qq.com
  * @version V2.0.0
  * @date    2023-05-25
  * @brief   
  ******************************************************************************
  */

/* Include -------------------------------------------------------------------*/
#include "bsp_i2c.h"
#include "sf_i2c.h"

/* Private function ----------------------------------------------------------*/
static void i2c0_sda_pin_low(void);
static void i2c0_sda_pin_high(void);
static void i2c0_scl_pin_low(void);
static void i2c0_scl_pin_high(void);
static uint8_t i2c0_sda_pin_read(void);
static void i2c0_sda_pin_dir_input(void);
static void i2c0_sda_pin_dir_output(void);


/**
 * @brief  i2c software delay function, used to control the i2c bus speed
 * @param  dev : Pointer to iic structure
 * @return none
 */
static void i2c_delay(const uint32_t us)
{
    __IO uint32_t i = us;

    while(i--);
}



/* Private variables ---------------------------------------------------------*/
// 定义i2c驱动对象
static struct sf_i2c_dev i2c0_dev = {
    .name               = "i2c0",
    .speed              = 2, /*! speed:105Hz */
    .delay_us           = i2c_delay,
    .ops.sda_low        = i2c0_sda_pin_low,
    .ops.sda_high       = i2c0_sda_pin_high,
    .ops.scl_low        = i2c0_scl_pin_low,
    .ops.scl_high       = i2c0_scl_pin_high,
    .ops.sda_read_level = i2c0_sda_pin_read,
    .ops.sda_set_input  = i2c0_sda_pin_dir_input,
    .ops.sda_set_output = i2c0_sda_pin_dir_output,
};

/*! Set i2c sda pin low level */
static void i2c0_sda_pin_low(void)
{
    GPIO_ResetBits(I2C0_PORT, I2C0_SDA_PIN);
}

/*! Set i2c sda pin high level */
static void i2c0_sda_pin_high(void)
{
    GPIO_SetBits(I2C0_PORT, I2C0_SDA_PIN);
}

/*! Set i2c scl pin low level */
static void i2c0_scl_pin_low(void)
{
    GPIO_ResetBits(I2C0_PORT, I2C0_SCL_PIN);
}

/*! Set i2c scl pin high level */
static void i2c0_scl_pin_high(void)
{
    GPIO_SetBits(I2C0_PORT, I2C0_SCL_PIN);
}

/*! Read i2c sda pin level */
static uint8_t i2c0_sda_pin_read(void)
{
    return GPIO_ReadInputDataBit(I2C0_PORT, I2C0_SDA_PIN);
}

/*! Switch i2c sda pin dir input */
static void i2c0_sda_pin_dir_input(void)
{
    I2C0_PORT->MODER &=0XFFFFFFFC;
}

/*! Switch i2c sda pin dir output */
static void i2c0_sda_pin_dir_output(void)
{
    I2C0_PORT->MODER &=0XFFFFFFFC;
    I2C0_PORT->MODER |= 0x01;
}

/**
 * @brief  Initialization i2c0 physical interface
 * @param  none
 * @return none
 */
static void i2c0_port_init(void)
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
    i2c0_port_init();

    /*! i2c software layer initialization */
    i2c_init(&i2c0_dev);
}
