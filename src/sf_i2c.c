/**
  ******************************************************************************
  * @file    drv_iic.c 
  * @author  Xiao Yang
  * @version V1.0.0
  * @date    2023-05-25
  * @brief   This file realizes the software simulation IIC driver library.
  * 
  *      _______. _______     __   ___     ______
  *     /       ||   ____|   |  | |__ \   /      |
  *    |   (----`|  |__      |  |    ) | |  ,----'
  *     \   \    |   __|     |  |   / /  |  |
  * .----)   |   |  |        |  |  / /_  |  `----.
  * |_______/    |__|        |__| |____|  \______|
  ******************************************************************************
  */

/*! Include--------------------------------------------------------------------*/
#include "sf_i2c.h"

#include <string.h>

/*******************************************************************************
 * i2c physical interface
 ******************************************************************************/
// SDA pin output low level
#define I2C_SDA_LOW(p_dev)          p_dev->ops.sda_low()

// SDA pin output high level
#define I2C_SDA_HIGH(p_dev)         p_dev->ops.sda_high()

// SCL pin output low level
#define I2C_SCL_LOW(p_dev)          p_dev->ops.scl_low()

// SCL pin output high level
#define I2C_SCL_HIGH(p_dev)         p_dev->ops.scl_high()

// Read SDA pin level
#define I2C_SDA_READ(p_dev)         p_dev->ops.sda_read_level()

// SDA pin input direction
#define I2C_SDA_INPUT(p_dev)        p_dev->ops.sda_set_input()

// SDA pin output direction
#define I2C_SDA_OUTPUT(p_dev)       p_dev->ops.sda_set_output();


/*******************************************************************************
 * linked list function
 ******************************************************************************/
// Insert node into linked list
#define INSERT_INTO(node) do {                          \
                              node->next = head_handle; \
                              head_handle = node;       \
                          } while (0)
                        

/*! Private function----------------------------------------------------------*/
static void sf_i2c_ack(struct sf_i2c_dev *dev);
static void sf_i2c_nack(struct sf_i2c_dev *dev);

/* Private variables ---------------------------------------------------------*/
#if (I2C_DEV_FIND > 0u)
static struct sf_i2c_dev* head_handle = NULL;
#endif

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


/**
 * @brief  Query device baseed on driver name
 * @param  dev_name : Pointer to driver name
 * @return i2c object pointer
 */
#if (I2C_DEV_FIND > 0u)
struct sf_i2c_dev *sf_i2c_dev_find(const char* name)
{
    struct sf_i2c_dev* TmpCell;

    /*! Query all nodes in the linked list */
    for(TmpCell = head_handle; TmpCell != NULL; TmpCell = TmpCell->next) {
        /*! Match driver name */
        if (strcmp(TmpCell->name, name) == 0) {
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
void sf_i2c_init(struct sf_i2c_dev *dev)
{
    // Initialzation pin high level
    I2C_SDA_HIGH(dev);
    I2C_SCL_HIGH(dev);

    #if (I2C_DEV_FIND > 0u)
    // Insert node into linked list
    INSERT_INTO(dev);
    #endif
}

/**
 * @brief  Generate start signal
 * @param  dev : Pointer to iic structure
 * @return none
 */
static void sf_i2c_start(struct sf_i2c_dev *dev)
{
    I2C_SCL_HIGH(dev);
    I2C_SDA_HIGH(dev);
    dev->delay_us(2);
    I2C_SDA_LOW(dev);
    dev->delay_us(2);
    I2C_SCL_LOW(dev);
}

static void sf_i2c_restart(struct sf_i2c_dev *dev)
{
    I2C_SDA_HIGH(dev);
    I2C_SCL_HIGH(dev);
    dev->delay_us(2);
    I2C_SDA_LOW(dev);
    dev->delay_us(2);
    I2C_SCL_LOW(dev);
}

/**
 * @brief  Generate stop signal
 * @param  dev : Pointer to iic structure
 * @return none
 */
static void sf_i2c_stop(struct sf_i2c_dev *dev)
{
    I2C_SCL_LOW(dev);
    I2C_SDA_LOW(dev);
    dev->delay_us(2);
    I2C_SCL_HIGH(dev);
    dev->delay_us(2);
    I2C_SDA_HIGH(dev);
    dev->delay_us(2);
}

/**
 * @brief  Generate response signal
 * @param  dev : Pointer to iic structure
 * @return none
 */
static void sf_i2c_ack(struct sf_i2c_dev *dev)
{
    I2C_SCL_LOW(dev);
    I2C_SDA_OUTPUT(dev);
    I2C_SDA_LOW(dev);
    dev->delay_us(dev->speed);
    I2C_SCL_HIGH(dev);
    dev->delay_us(dev->speed);
    I2C_SCL_LOW(dev);
}

/**
 * @brief  Generate no response signal
 * @param  dev : Pointer to iic structure
 * @return none
 */
static void sf_i2c_nack(struct sf_i2c_dev *dev)
{
    I2C_SCL_LOW(dev);
    I2C_SDA_OUTPUT(dev);
    I2C_SDA_LOW(dev);
    dev->delay_us(dev->speed);
    I2C_SDA_HIGH(dev);
    dev->delay_us(dev->speed);
    I2C_SCL_HIGH(dev);
    dev->delay_us(dev->speed);
    I2C_SCL_LOW(dev);
}

/**
 * @brief  Detection response signal
 * @param  dev         : Pointer to iic structure
 * @return I2C_SUCCESS : Not error
 *         IIC_TIMEOUT : Timeout,Device response is not received
 */
static I2C_Error_t sf_i2c_wait_ack(struct sf_i2c_dev *dev)
{
    uint16_t wait_time = 0xFFF;

    I2C_SCL_LOW(dev);
    I2C_SDA_INPUT(dev);
    dev->delay_us(dev->speed);
    I2C_SCL_HIGH(dev);
    while (I2C_SDA_READ(dev)) {
        if ((wait_time--) == 0) {
            I2C_SDA_HIGH(dev);
            I2C_SDA_OUTPUT(dev);
            sf_i2c_stop(dev);
            return I2C_TIMEOUT;
        }
    }
    dev->delay_us(dev->speed);
    I2C_SCL_LOW(dev);
    I2C_SDA_HIGH(dev);
    I2C_SDA_OUTPUT(dev);
    return I2C_SUCCESS;
}

/**
 * @brief  Write a byte data to the I2C bus
 * @param  dev         : Pointer to iic structure
 * @param  byte        : Data write to the iic bus
 * @return IIC_SUCCESS : Not error
 *         I2C_TIMEOUT : Timeout,Device response is not received
 */
I2C_Error_t sf_i2c_send_byte(struct sf_i2c_dev *dev, uint8_t byte)
{
    uint8_t i;

    for (i = 0; i < 8; i++) {
        I2C_SCL_LOW(dev);
        dev->delay_us(dev->speed);
        if (byte & 0x80)
            I2C_SDA_HIGH(dev);
        else
            I2C_SDA_LOW(dev);
        I2C_SCL_HIGH(dev);
        dev->delay_us(dev->speed);
        byte <<= 1;
    }

    return sf_i2c_wait_ack(dev);
}

/**
 * @brief  Read a byte data from the I2C bus
 * @param  dev : pointer to iic structure
 * @param  ack : indicates whether to return a response signal after data acceptance is completed
 * @return the data read on the iic bus
 */
static uint8_t sf_i2c_read_byte(struct sf_i2c_dev *dev, uint8_t ack)
{
    uint8_t i, byte = 0;

    I2C_SDA_INPUT(dev);
    for (i = 0; i < 8; i++) {
        byte <<= 1;
        I2C_SCL_LOW(dev);
        dev->delay_us(dev->speed);
        I2C_SCL_HIGH(dev);
        if (I2C_SDA_READ(dev))
            byte |= 0x01;
        dev->delay_us(dev->speed);
    }
    if (ack)
        sf_i2c_ack(dev);
    else
        sf_i2c_nack(dev);
    return byte;
}

/**
 * @brief  Write mult byte data to the I2C bus
 * @param  dev         : pointer to iic structure
 * @param  msg         : pointer to message struct
 * @return IIC_SUCCESS : Not error
 *         I2C_TIMEOUT : Timeout,Device response is not received
 */
static I2C_Error_t sf_i2c_send_bytes(struct sf_i2c_dev *dev, struct sf_i2c_msg *msg)
{
    I2C_Error_t ret;
    const uint8_t *ptr = msg->buf;
    int count = msg->len;
    
    while (count--) {
        ret = sf_i2c_send_byte(dev, *ptr++);
    }
    
    return ret;
}

/**
 * @brief  Read mult byte data from the I2C bus
 * @param  dev : pointer to iic structure
 * @param  msg : pointer to message struct
 * @return the data read on the iic bus
 */
static I2C_Error_t sf_i2c_read_bytes(struct sf_i2c_dev *dev, struct sf_i2c_msg *msg)
{
    I2C_Error_t ret = I2C_SUCCESS;
    uint8_t *ptr = msg->buf;
    int count = msg->len;
    
    while (count--) {
        if (count == 0) {
            *ptr++ = sf_i2c_read_byte(dev, 0);
        } else {
            *ptr++ = sf_i2c_read_byte(dev, 1);
        }
    }

    return ret;
}

/**
 * @brief  Send 7bit target addr to I2C bus
 * @param  dev : pointer to iic structure
 * @param  msg : pointer to message struct
 * @return the data read on the iic bus
 */
static I2C_Error_t sf_i2c_send_addr(struct sf_i2c_dev *dev, struct sf_i2c_msg *msg)
{
    I2C_Error_t ret;
    uint8_t addr = msg->addr;
    
    if (msg->flags & SF_I2C_FLAG_WR) {
        addr &= 0xFE;
        ret = sf_i2c_send_byte(dev, addr);
    } else {
        addr |= 0x01;
        ret = sf_i2c_send_byte(dev, addr);
    }
    return ret;
}

/**
 * @brief  Transport messages to I2C bus
 * @param  dev    : Pointer to iic structure
 * @param  msgs   : Pointer to transport message structure
 * @param  num    : transport message count
 * @return none
 */
I2C_Error_t sf_i2c_transfer(struct sf_i2c_dev *dev, struct sf_i2c_msg *msgs, uint16_t num)
{
    I2C_Error_t ret;
    struct sf_i2c_msg *msg;
    
    for (int i = 0; i < num; i++) {
        msg = &msgs[i];
        if (!(msg->flags & SF_I2C_FLAG_NO_START)) {
            if (i == 1) {
                sf_i2c_restart(dev);
            } else {
                sf_i2c_start(dev);
            }
            
            ret = sf_i2c_send_addr(dev, msg);
        }
        
        if (msg->flags & SF_I2C_FLAG_WR) {
            ret = sf_i2c_send_bytes(dev, msg);
        } else {
            ret = sf_i2c_read_bytes(dev, msg);
        }
        
        if (ret == I2C_TIMEOUT)
            break;
    }
    
    sf_i2c_stop(dev);
    
    return ret;
}
