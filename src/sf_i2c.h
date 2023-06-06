/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SF_I2C_H
#define __SF_I2C_H

/* Include -------------------------------------------------------------------*/
#include <stdint.h>

/* Public define -------------------------------------------------------------*/
#define I2C_DEV_FIND            1u         /*! Find i2c device object */

/**
 * i2c error infomation definition
 */ 
typedef enum
{
    I2C_SUCCESS = 0, /*! Not error   */
    I2C_TIMEOUT,     /*! ack timeout */
} I2C_Error_t;

/**
 * i2c gpio optinos struct definition
 */ 
typedef struct
{
    void (*sda_low)(void);              /*! Set i2c sda pin low level     */
    void (*sda_high)(void);             /*! Set i2c sda pin high level    */
    void (*scl_low)(void);              /*! Set i2c scl pin low level     */
    void (*scl_high)(void);             /*! Set i2c scl pin high level    */
    uint8_t (*sda_read_level)(void);    /*! Read i2c sda pin level        */
    void (*sda_set_input)(void);        /*! Switch i2c sda pin dir input  */
    void (*sda_set_output)(void);       /*! Switch i2c sda pin dir output */
} i2c_ops_t;

/**
 * I2C driver struct definition
 */
struct sf_i2c_dev
{
    i2c_ops_t ops;                       /*! i2c ops interface    */
    uint32_t speed;                      /*! control i2c bus speed */
    void (*delay_us)(uint32_t);
#if (I2C_DEV_FIND > 0u)                  /*! if enable device find */
    const char *name;                    /*! i2c device name       */
    struct sf_i2c_dev *next;             /*! For the linked list   */
#endif
};
typedef struct sf_i2c_dev * sf_i2c_dev_t;


/**
 * I2C transfer flag definition
 */
#define SF_I2C_FLAG_WR              0x0001
#define SF_I2C_FLAG_RD              0x0002
#define SF_I2C_FLAG_NO_START        0x0004

/**
 * I2C transport message struct
 */
struct sf_i2c_msg
{
    uint16_t addr;                      /*! traget 8bit addr */
    uint16_t flags;                     /*! transport flag */
    uint16_t len;                       /*! transport buffer size */
    uint8_t *buf;                       /*! transport byte buffer */
};
typedef struct sf_i2c_msg sf_i2c_msg_t;

/* Exported functions --------------------------------------------------------*/
void         sf_i2c_init(struct sf_i2c_dev *dev);
#if (I2C_DEV_FIND > 0u)
sf_i2c_dev_t sf_i2c_dev_find(const char* name);
#endif
I2C_Error_t  sf_i2c_transfer(struct sf_i2c_dev *dev, struct sf_i2c_msg *msgs, uint16_t num);

#endif /* __SF_I2C_H */
