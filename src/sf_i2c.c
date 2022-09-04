/**
  ******************************************************************************
  * @file    sf_i2c.c 
  * @author  Xiao Yang 260384793@qq.com
  * @version V1.0.0
  * @date    2021-10-06
  * @brief   This file realizes the software simulation IIC driver library.
  ******************************************************************************
  */

/*! Include--------------------------------------------------------------------*/
#include "sf_i2c.h"

#include <string.h>

//-----------------------------------------------------------------------------
// i2c physical interface
//-----------------------------------------------------------------------------
// SDA pin output low level
#define I2C_PIN_SDA_LOW(p_dev) \
                        p_dev->port.sda_pin_out_low()

// SDA pin output high level
#define I2C_PIN_SDA_HIGH(p_dev) \
                        p_dev->port.sda_pin_out_high()

// SCL pin output low level
#define I2C_PIN_SCL_LOW(p_dev) \
                        p_dev->port.scl_pin_out_low()

// SCL pin output high level
#define I2C_PIN_SCL_HIGH(p_dev) \
                        p_dev->port.scl_pin_out_high()

// Read SDA pin level
#define I2C_PIN_SDA_READ(p_dev) \
                        p_dev->port.sda_pin_read_level()

// SDA pin input direction
#define I2C_PIN_SDA_DIR_INPUT(p_dev) \
                        p_dev->port.sda_pin_dir_input()

// SDA pin output direction
#define I2C_PIN_SDA_DIR_OUTPUT(p_dev) \
                        p_dev->port.sda_pin_dir_output();



//-----------------------------------------------------------------------------
// linked list function
//-----------------------------------------------------------------------------
// Insert node into linked list
#define INSERT_INTO(node) do {                          \
                              node->next = head_handle; \
                              head_handle = node;       \
                          } while (0)
                        

/*! Private function----------------------------------------------------------*/
static void i2c_delay(const i2c_dev *dev);
static void i2c_ack(const i2c_dev *dev);
static void i2c_nack(const i2c_dev *dev);

/* Private variables ---------------------------------------------------------*/
#if (I2C_OBJ_FIND > 0u)
static i2c_dev* head_handle = NULL;
#endif

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  i2c software delay function, used to control the i2c bus speed
 * @param  dev : Pointer to iic structure
 * @return none
 */
static void i2c_delay(const i2c_dev *dev)
{
    volatile uint32_t i = dev->speed;

    while (i--);
}

/**
 * @brief  Query object baseed on driver name
 * @param  dev_name : Pointer to driver name
 * @return i2c object pointer
 */
#if (I2C_OBJ_FIND > 0u)
i2c_dev *i2c_obj_find(const char* dev_name)
{
    i2c_dev* TmpCell;

    /*! Query all nodes in the linked list */
    for (TmpCell = head_handle; TmpCell != NULL; TmpCell = TmpCell->next) {
        /*! Match driver name */
        if (strcmp(TmpCell->name, dev_name) == 0) {
            return TmpCell;
        }
    }
    return NULL;
}
#endif

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
void i2c_init(i2c_dev *dev)
{
    // Initialzation pin high level
    I2C_PIN_SDA_HIGH(dev);
    I2C_PIN_SCL_HIGH(dev);

    #if (I2C_OBJ_FIND > 0u)
    // Insert node into linked list
    INSERT_INTO(dev);
    #endif
}

/**
 * @brief  Generate start signal
 * @param  dev : Pointer to iic structure
 * @return none
 */
void i2c_start(const i2c_dev *dev)
{
    I2C_PIN_SCL_HIGH(dev);
    I2C_PIN_SDA_HIGH(dev);
    i2c_delay(dev);
    I2C_PIN_SDA_LOW(dev);
    i2c_delay(dev);
    I2C_PIN_SCL_LOW(dev);
}

/**
 * @brief  Generate stop signal
 * @param  dev : Pointer to iic structure
 * @return none
 */
void i2c_stop(const i2c_dev *dev)
{
    I2C_PIN_SCL_LOW(dev);
    I2C_PIN_SDA_LOW(dev);
    i2c_delay(dev);
    I2C_PIN_SCL_HIGH(dev);
    i2c_delay(dev);
    I2C_PIN_SDA_HIGH(dev);
    i2c_delay(dev);
}

/**
 * @brief  Generate response signal
 * @param  dev : Pointer to iic structure
 * @return none
 */
static void i2c_ack(const i2c_dev *dev)
{
    I2C_PIN_SCL_LOW(dev);
    I2C_PIN_SDA_DIR_OUTPUT(dev);
    I2C_PIN_SDA_LOW(dev);
    i2c_delay(dev);
    I2C_PIN_SCL_HIGH(dev);
    i2c_delay(dev);
    I2C_PIN_SCL_LOW(dev);
}

/**
 * @brief  Generate no response signal
 * @param  dev : Pointer to iic structure
 * @return none
 */
static void i2c_nack(const i2c_dev *dev)
{
    I2C_PIN_SCL_LOW(dev);
    I2C_PIN_SDA_DIR_OUTPUT(dev);
    I2C_PIN_SDA_LOW(dev);
    i2c_delay(dev);
    I2C_PIN_SDA_HIGH(dev);
    i2c_delay(dev);
    I2C_PIN_SCL_HIGH(dev);
    i2c_delay(dev);
    I2C_PIN_SCL_LOW(dev);
}

/**
 * @brief  Detection response signal
 * @param  dev         : Pointer to iic structure
 * @return SF_I2C_SUCCESS : Not error
 *         IIC_TIMEOUT : Timeout,Device response is not received
 */
static sf_i2c_err i2c_wait_ack(const i2c_dev *dev)
{
    uint16_t wait_time = 255;

    I2C_PIN_SCL_LOW(dev);
    I2C_PIN_SDA_DIR_INPUT(dev);
    i2c_delay(dev);
    I2C_PIN_SCL_HIGH(dev);
    while (I2C_PIN_SDA_READ(dev)) {
        if ((wait_time--) == 0) {
            I2C_PIN_SDA_HIGH(dev);
            I2C_PIN_SDA_DIR_OUTPUT(dev);
            i2c_stop(dev);
            return SF_I2C_TIMEOUT;
        }
    }
    i2c_delay(dev);
    I2C_PIN_SCL_LOW(dev);
    I2C_PIN_SDA_HIGH(dev);
    I2C_PIN_SDA_DIR_OUTPUT(dev);
    return SF_I2C_SUCCESS;
}

/**
 * @brief  Write a byte data to the I2C bus
 * @param  dev         : Pointer to iic structure
 * @param  byte        : Data write to the iic bus
 * @return IIC_SUCCESS : Not error
 *         SF_I2C_TIMEOUT : Timeout,Device response is not received
 */
sf_i2c_err i2c_write_byte(const i2c_dev *dev, uint8_t byte)
{
    uint8_t i;
    sf_i2c_err err;

    for (i = 0; i < 8; i++) {
        I2C_PIN_SCL_LOW(dev);
        i2c_delay(dev);
        if (byte & 0x80)
            I2C_PIN_SDA_HIGH(dev);
        else
            I2C_PIN_SDA_LOW(dev);
        I2C_PIN_SCL_HIGH(dev);
        i2c_delay(dev);
        byte <<= 1;
    }
    err = i2c_wait_ack(dev);
    return err;
}

/**
 * @brief  Read a byte data from the I2C bus
 * @param  dev : Pointer to iic structure
 * @param  ack : Indicates whether to return a response signal after data acceptance is completed
 * @return the data read on the iic bus
 */
uint8_t i2c_read_byte(const i2c_dev *dev, uint8_t ack)
{
    uint8_t i, byte = 0;

    I2C_PIN_SDA_DIR_INPUT(dev);
    for (i = 0; i < 8; i++) {
        byte <<= 1;
        I2C_PIN_SCL_LOW(dev);
        i2c_delay(dev);
        I2C_PIN_SCL_HIGH(dev);
        if (I2C_PIN_SDA_READ(dev))
            byte |= 0x01;
        i2c_delay(dev);
    }
    if (ack)
        i2c_ack(dev);
    else
        i2c_nack(dev);
    return byte;
}

/**
 * @brief  i2c writes multiple bytes to a register consecutively
 * @param  dev Pointer : to iic structure
 * @param  slave_addr  : Device address
 * @param  reg_addr    : Register address
 * @param  pbuf        : Pointer to source buffer
 * @param  length      : The number of bytes that need to be write
 * @return IIC_SUCCESS : Not error
 *         IIC_TIMEOUT : Timeout,Device response is not received
 */
sf_i2c_err i2c_write_multi_byte(const i2c_dev *dev, uint8_t slave_addr, 
                                 uint8_t reg_addr, void *pbuf, uint16_t length)
{
    uint8_t i;
    sf_i2c_err err;
    uint8_t *p = (uint8_t*)pbuf;

    i2c_start(dev);
    i2c_write_byte(dev, I2C_WRITE(slave_addr));
    i2c_write_byte(dev, reg_addr);

    for (i = 0; i < length; i++) {
        err = i2c_write_byte(dev, p[i]);
    }
    i2c_stop(dev);
    return err;
}

/**
 * @brief  i2c reads multiple bytes consecutively from a register
 * @param  dev          : Pointer to iic structure
 * @param  slave_addr   : Device address
 * @param  reg_addr     : Register address
 * @param  pbuf         : Pointer to target buffer
 * @param  length       : The number of bytes that need to be read
 * @return none
 */
void i2c_read_multi_byte(const i2c_dev *dev, uint8_t slave_addr, 
                         uint8_t reg_addr, void *pbuf, uint16_t length)
{
    uint8_t i;
    uint8_t *p = (uint8_t*)pbuf;

    i2c_start(dev);
    i2c_write_byte(dev, I2C_WRITE(slave_addr));
    i2c_write_byte(dev, reg_addr);
    i2c_start(dev);
    i2c_write_byte(dev, I2C_READ(slave_addr));

    for (i = 0; i < length; i++) {
        if (i != (length - 1)) {
            p[i] = i2c_read_byte(dev, 1);
        } else {
            p[i] = i2c_read_byte(dev, 0);
        }
    }
    i2c_stop(dev);
}

/**
 * @brief  i2c writes multiple bytes to a register consecutively
 * @param  dev Pointer : to iic structure
 * @param  slave_addr  : Device address
 * @param  reg_addr    : Register address
 * @param  pbuf        : Pointer to source buffer
 * @param  length      : The number of bytes that need to be write
 * @return IIC_SUCCESS : Not error
 *         IIC_TIMEOUT : Timeout,Device response is not received
 */
sf_i2c_err i2c_write_multi_byte_16bit(const i2c_dev *dev, uint8_t slave_addr, 
                                      uint16_t reg_addr, void *pbuf, uint16_t length)
{
    uint8_t i;
    sf_i2c_err err;
    uint8_t *p = (uint8_t*)pbuf;

    i2c_start(dev);
    i2c_write_byte(dev, I2C_WRITE(slave_addr));
    i2c_write_byte(dev, (uint8_t)(reg_addr >> 8));
    i2c_write_byte(dev, (uint8_t)reg_addr);

    for (i = 0; i < length; i++) {
        err = i2c_write_byte(dev, p[i]);
    }
    i2c_stop(dev);
    return err;
}

/**
 * @brief  i2c reads multiple bytes consecutively from a register
 * @param  dev          : Pointer to iic structure
 * @param  slave_addr   : Device address
 * @param  reg_addr     : Register address
 * @param  pbuf         : Pointer to target buffer
 * @param  length       : The number of bytes that need to be read
 * @return none
 */
void i2c_read_multi_byte_16bit(const i2c_dev *dev, uint8_t slave_addr, 
                               uint16_t reg_addr, void *pbuf, uint16_t length)
{
    uint8_t i;
    uint8_t *p = (uint8_t*)pbuf;

    i2c_start(dev);
    i2c_write_byte(dev, I2C_WRITE(slave_addr));
    i2c_write_byte(dev, (uint8_t)(reg_addr >> 8));
    i2c_write_byte(dev, (uint8_t)reg_addr);
    i2c_start(dev);
    i2c_write_byte(dev, I2C_READ(slave_addr));

    for (i = 0; i < length; i++) {
        if (i != (length - 1)) {
            p[i] = i2c_read_byte(dev, 1);
        } else {
            p[i] = i2c_read_byte(dev, 0);
        }
    }
    i2c_stop(dev);
}
