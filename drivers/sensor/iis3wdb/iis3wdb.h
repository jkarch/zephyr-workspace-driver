#ifndef IIS3WDB_H
#define IIS3WDB_H
#include <stdint.h>


/* Pins Definition for iis3wdb */
#define SPI_SS_PIN   25  // pin to be connected to cs(chip select pin)
#define SPI_SCK_PIN  22  // pin to be connected to clock pin (scl)
#define SPI_MISO_PIN 24  // pin to be connected to MISO 
#define SPI_MOSI_PIN 23  // pin to be connected to MOSI


#define    SPIFlash_CS_LOW    nrf_gpio_pin_clear(SPI_SS_PIN)   // a simple function call to clear the cs pin
#define    SPIFlash_CS_HIGH   nrf_gpio_pin_set(SPI_SS_PIN)     //a simple function call to set the cs pin 


/* IIS3WDB registers addresses */
#define ADD_REG_WHO_AM_I				0x0F
#define ADD_REG_CTRL_1					0x10
#define ADD_REG_CTRL_6					0x20
#define ADD_REG_OUT_X_L					0x28
#define ADD_REG_OUT_X_H					0x29
#define ADD_REG_OUT_Y_L					0x2A
#define ADD_REG_OUT_Y_H					0x2B
#define ADD_REG_OUT_Z_L					0x2C
#define ADD_REG_OUT_Z_H					0x2D

/* WHO AM I register default value */
#define UC_WHO_AM_I_DEFAULT_VALUE		0x7B

/* ADD_REG_CTRL_6 register configuration value: X,Y,Z axis enabled and 400Hz of output data rate
   for more info see the datasheet of iis3wdb
 */
#define UC_ADD_REG_CTRL_6_CFG_VALUE		0x00
//Set 2G, first stage digital filter, accel enabled
#define UC_ADD_REG_CTRL_1_CFG_VALUE		0xA0

/* Sensitivity for 2G range [mg/digit] */
#define SENS_2G_RANGE_MG_PER_DIGIT		((float)0.061)

/* LED threshold value in mg UNUSED*/
#define LED_TH_MG						(1000)	/* 1000mg (1G) */


/* ---------------- Local Macros ----------------- */


/* set read single command. Attention: command must be 0x3F at most */
#define SET_READ_SINGLE_CMD(x)			(x | 0x80)
/* set read multiple command. Attention: command must be 0x3F at most */
#define SET_READ_MULTI_CMD(x)			(x | 0xC0)
/* set write single command. Attention: command must be 0x3F at most */
#define SET_WRITE_SINGLE_CMD(x)			(x & (~(0xC0)))
/* set write multiple command. Attention: command must be 0x3F at most */
#define SET_WRITE_MULTI_CMD(x)			(x & (~(0x80))	\
                                                 x |= 0x40)

#define SPI_BUFSIZE  8   //SPI Communication buffer size
#define SPI_INSTANCE  0 //SPI Instance to be used
#define FRAME_SIZE 8

#define SPI_OP(frame_size) SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_WORD_SET(frame_size) | SPI_LINES_SINGLE

volatile  uint8_t   SPIReadLength, SPIWriteLength; // variables to hold read and write lengths

struct iis3wdb_data{
    uint8_t   spi_tx_buf[SPI_BUFSIZE]; // spi tx buffer 
    uint8_t   spi_rx_buf[SPI_INSTANCE]; // spi rx buffer

    const struct device *bus;

    int x,y,z; //values of acceleration in x,y & z
};

union iis3wdb_bus_cfg{
	struct spi_dt_spec spi;
};

struct iis3wdb_config {
	int (*bus_init)(const struct device *dev);
	const union iis3wdb_bus_cfg bus_cfg;
};

int twoComplToInt16(int twoComplValue);
static void IIS3WDB_write_reg(const struct device *dev,int reg, int val);
int IIS3WDB_read_reg(const struct device *dev,int reg);
int SPI_init(const struct device *dev);
static int iis3wdb_init (const struct device *dev);
int IIS3WDB_chip_init(const struct device *dev);
static int iis3wdb_channel_get(const struct device *dev, enum sensor_channel chan,struct sensor_value *val);
static int get_acceleration( const struct device *dev);


#endif
/********************************************END FILE*******************************************/