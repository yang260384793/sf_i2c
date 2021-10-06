# sf_i2c

#### 简介
sf_i2c是一个可移植性高的软件模拟i2c驱动库，可以移植到任何单片机代码中，基于多驱动设计，底层驱动与接口完全分离，可在一个单片机软件中创建多个i2c接口，每个i2c接口用名称来进行区分，可通过名称查找i2c接口的指针，只需要实现一些简单的接口就可以使用。


#### 使用方法

1.	创建i2c驱动结构体
```
static i2c_dev i2c0_dev = {
	.name                    = "i2c0",
	.speed                   = 40,                      /*! speed:120Hz */
	.port.sda_pin_out_low    = i2c0_sda_pin_out_low,
	.port.sda_pin_out_high   = i2c0_sda_pin_out_high,
	.port.scl_pin_out_low    = i2c0_scl_pin_out_low,
	.port.scl_pin_out_high   = i2c0_scl_pin_out_high,
	.port.sda_pin_read_level = i2c0_sda_pin_read_level,
	.port.sda_pin_dir_input  = i2c0_sda_pin_dir_input,
	.port.sda_pin_dir_output = i2c0_sda_pin_dir_output,
};
```
2.  设置i2c总线速度
```
static i2c_dev i2c0_dev = {
	...
    //修改此处可调整总线速度，没有固定值，需根据不同单片机时钟速度来做调整
	.speed                   = 40, 
	...
};
```
3.  实现i2c0_sda_pin_out_low
```
__STATIC_INLINE void i2c0_sda_pin_out_low(void)
{
	//设置SDA引脚输出低电平
}
```
4.	实现i2c0_sda_pin_out_high
```
__STATIC_INLINE void i2c0_sda_pin_out_high(void)
{
	//设置SDA引脚输出高电平
}
```
5.	实现i2c0_scl_pin_out_low
```
__STATIC_INLINE void i2c0_scl_pin_out_low(void)
{
	//设置SCL引脚输出低电平
}
```
6.	实现i2c0_scl_pin_out_high
```
__STATIC_INLINE void i2c0_scl_pin_out_high(void)
{
	//设置SCL引脚输出高电平
}
```
7.	实现i2c0_sda_pin_read_level
```
__STATIC_INLINE uint8_t i2c0_sda_pin_read_level(void)
{
	//返回SDA引脚电平状态
}
```
8.	实现i2c0_sda_pin_dir_input
```
__STATIC_INLINE void i2c0_sda_pin_dir_input(void)
{
	//设置SDA引脚输入方向
}
```
9.	实现i2c0_sda_pin_dir_output
```
__STATIC_INLINE void i2c0_sda_pin_dir_output(void)
{
	//设置SDA引脚输出方向
}
```
10.  初始化i2c
```
调用i2c_init函数，传入创建的i2c驱动指针，初始化完成后可进行其它操作。
i2c_init(&i2c0_dev);
```
11.  获取i2c驱动指针
```
调用i2c_obj_find函数，传入i2c驱动的名称，可获取到名称对应的i2c驱动的指针。
i2c_dev *obj = i2c_obj_find("i2c0");
```

#### Examples

```
static void i2c0_phy_init(void)
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
    i2c0_phy_init();

    /*! i2c software layer initialization */
    i2c_init(&i2c0_dev);
}
```

