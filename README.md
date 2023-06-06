# sf_i2c

## 项目介绍
&emsp;&emsp; 一个可移植性高的软件模拟i2c驱动库，可以移植到任何单片机代码中，基于多驱动设计，底层驱动与接口完全分离，可在一个单片机软件中创建多个i2c接口，每个i2c接口用名称来进行区分，可通过名称查找i2c接口的指针，只需要实现一些简单的接口就可以使用。


## 使用方法

##### 1.	创建i2c驱动结构体
```c
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
```
##### 2.  设置i2c总线速度
```c
static struct sf_i2c_dev i2c0_dev = {
	...
    //修改此处可调整总线速度，没有固定值，需根据不同单片机时钟速度来做调整
	.speed                   = 40, 
	...
};
```
##### 3.  实现i2c0_sda_pin_out_low
```c
static void i2c0_sda_pin_low(void)
{
	//设置SDA引脚输出低电平
}
```
##### 4.	实现i2c0_sda_pin_out_high
```c
static void i2c0_sda_pin_high(void)
{
	//设置SDA引脚输出高电平
}
```
##### 5.	实现i2c0_scl_pin_out_low
```c
static void i2c0_scl_pin_low(void)
{
	//设置SCL引脚输出低电平
}
```
##### 6.	实现i2c0_scl_pin_out_high
```c
static void i2c0_scl_pin_high(void)
{
	//设置SCL引脚输出高电平
}
```
##### 7.	实现i2c0_sda_pin_read_level
```c
static uint8_t i2c0_sda_pin_read(void)
{
	//返回SDA引脚电平状态
}
```
##### 8.	实现i2c0_sda_pin_dir_input
```c
static void i2c0_sda_pin_dir_input(void)
{
	//设置SDA引脚输入方向
}
```
##### 9.	实现i2c0_sda_pin_dir_output
```c
static void i2c0_sda_pin_dir_output(void)
{
	//设置SDA引脚输出方向
}
```
##### 10.  初始化i2c
调用i2c_init函数，传入创建的i2c驱动指针，初始化完成后可进行其它操作。
```c
i2c_init(&i2c0_dev);
```
##### 11.  获取i2c驱动指针
调用i2c_obj_find函数，传入i2c驱动的名称，可获取到名称对应的i2c驱动的指针。
```c
struct sf_i2c_dev *dev = sf_i2c_dev_find("i2c0");
```

## Examples

1.初始化i2c驱动接口

```c
static void i2c0_port_init(void)
{
    // Config pin output direction
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(I2C0_PORT_CLOCK, ENABLE);

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

void bsp_i2c_init(void)
{
    /*! i2c physical layer initialization */
    i2c0_port_init();

    /*! i2c software layer initialization */
    i2c_init(&i2c0_dev);
}
```

2.写数据测试

```
const char test_data[5] = {0x01, 0x02, 0x03, 0x04, 0x05};

// 向器件地址0xA0地址0x01连续写多个字节数据
void master_write_test(void)
{
	char reg_addr = 0x01;
	struct sf_i2c_msg msgs[2];
	
	// 写目标寄存器地址
	msgs[0].addr  = 0xA0;
    msgs[0].flags = SF_I2C_FLAG_WR;
    msgs[0].buf   = &reg_addr;
    msgs[0].len   = sizeof (reg_addr);
    
    // 向目标寄存器地址连续写多个数据
    msgs[1].addr  = 0xA0;
    msgs[1].flags = SF_I2C_FLAG_WR | SF_I2C_FLAG_NO_START;
    msgs[1].buf   = test_data;
    msgs[1].len   = sizeof (test_data);
    sf_i2c_transfer(dev, msgs, 2);
}
```

3.读数据测试

```
const char test_data[5];

// 向器件地址0xA0地址0x01连续写多个字节数据
void master_read_test(void)
{
	char reg_addr = 0x01;
	struct sf_i2c_msg msgs[2];
	
	// 写目标寄存器地址
	msgs[0].addr  = 0xA0;
    msgs[0].flags = SF_I2C_FLAG_WR;
    msgs[0].buf   = &reg_addr;
    msgs[0].len   = sizeof (reg_addr);
    
    // 从目标寄存器地址连续读多个数据
    msgs[1].addr  = 0xA0;
    msgs[1].flags = SF_I2C_FLAG_RD;
    msgs[1].buf   = test_data;
    msgs[1].len   = sizeof (test_data);
    sf_i2c_transfer(dev, msgs, 2);
}
```

