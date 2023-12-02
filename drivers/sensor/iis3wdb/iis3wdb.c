#define DT_DRV_COMPAT st_iis3wdb

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/spi.h>
#include "iis3wdb.h"


//Helper functions 
/* A function to convert two's complement value to 16-bit int value */
int twoComplToInt16(int twoComplValue)
{
	  int int16Value = 0;

	  /* conversion */
	  if(twoComplValue > 32768)
	  {
		    int16Value = -(((~twoComplValue) & 0xFFFF) + 1);
	  }
	  else
	  {
		    int16Value = twoComplValue;
	  }
	  return int16Value;
}

static void IIS3WDB_write_reg(const struct device *dev,int reg, int val)
{
    struct iis3wdb_data *data = dev->data;
    const struct iis3wdb_config *cfg = dev->config;



    SPIWriteLength = 2; // set the spi write length to 2 bytes
    SPIReadLength = 0; // set the read length

    data->spi_tx_buf[0] = SET_WRITE_SINGLE_CMD(reg); // set the first byte which is a write command
    data->spi_tx_buf[1] = val; // A byte of data to be sent
	
    //declare them in every read write function
    struct spi_buf tx_buf_arr; 
    struct spi_buf_set tx;
    struct spi_buf rx_buf_arr;
    struct spi_buf_set rx ;


   //specifying the tx and rx buffer specific to zephyr's SPI drivers
   	tx_buf_arr.buf = data->spi_tx_buf;
    tx_buf_arr.len = SPIWriteLength;
    tx.buffers = &tx_buf_arr;
    tx.count = 1;
	
    rx_buf_arr.buf = data->spi_rx_buf;
    rx_buf_arr.len = SPIReadLength;

    rx.buffers = &rx_buf_arr ;
    rx.count = 1;

	int error = spi_transceive_dt(&cfg->bus_cfg.spi, &tx, &rx);

	if(error != 0){
		printk("SPI transceive error: %i\n", error);
	}
   
}


int IIS3WDB_read_reg(const struct device *dev,int reg)
{	
    /* Set the read command for reading a single byte */

    struct iis3wdb_data *data = dev->data;
    const struct iis3wdb_config *cfg = dev->config;


    data->spi_tx_buf[0] = SET_READ_SINGLE_CMD(reg);  
  
    //declare them in every read write function
    struct spi_buf tx_buf_arr; 
    struct spi_buf_set tx;
    struct spi_buf rx_buf_arr;
    struct spi_buf_set rx ;



   //specifying the tx and rx buffer specific to zephyr's SPI drivers
   	tx_buf_arr.buf = data->spi_tx_buf;
    tx_buf_arr.len = 2;

    tx.buffers = &tx_buf_arr;
    tx.count = 1;
	
    rx_buf_arr.buf = data->spi_rx_buf;
    rx_buf_arr.len = 2;

    rx.buffers = &rx_buf_arr ;
    rx.count = 1;

	int error = spi_transceive_dt(&cfg->bus_cfg.spi, &tx, &rx);
	if(error != 0){
		printk("SPI transceive error: %i\n", error);
		return error;
	}

    return data->spi_rx_buf[1];
}


int SPI_init(const struct device *dev)
{

    struct iis3wdb_data *data = dev->data;
    const struct iis3wdb_config *cfg = dev->config;

    if(!spi_is_ready(&cfg->bus_cfg.spi)){
        printk("SPI bus is not ready \r\n");
    }

    
    return 0;
}


static int iis3wdb_init (const struct device *dev)
{
    const struct iis3wdb_config *cfg = dev->config;
    struct iis3wdb_data *data = dev->data;

    int status = cfg->bus_init(dev);
	if (status < 0) {
		return status;
	}
    IIS3WDB_chip_init(dev);

    return 0;
}



/* A function to initialize the Lis3dsh Sensor chip*/
int IIS3WDB_chip_init(const struct device *dev)
{        
    //initialising chip
    int regval;

    /* Read the who am i register to confirm iis3wdb is communicating with our nrf device */
    regval = IIS3WDB_read_reg(dev,ADD_REG_WHO_AM_I);

    /* check the who am i register value and compare it with default who am i value for iis3wdb 
        if who am i is correct then proced */
    if( regval == UC_WHO_AM_I_DEFAULT_VALUE )
    {
		IIS3WDB_write_reg(dev,ADD_REG_CTRL_1, UC_ADD_REG_CTRL_1_CFG_VALUE);

            /* verify written value */
        regval = IIS3WDB_read_reg(dev,ADD_REG_CTRL_1);

            /* if written value is different */
        if( regval != UC_ADD_REG_CTRL_1_CFG_VALUE )
        {
                printk("\r\nWrite Reg ERR\r\n");
                k_msleep(5000);
        }
        IIS3WDB_write_reg(dev,ADD_REG_CTRL_6, UC_ADD_REG_CTRL_6_CFG_VALUE);

            /* verify written value */
        regval = IIS3WDB_read_reg(dev,ADD_REG_CTRL_6);

            /* if written value is different */
        if( regval != UC_ADD_REG_CTRL_6_CFG_VALUE )
        {
                printk("\r\nWrite Reg ERR\r\n");
                k_msleep(5000);
        }
    }
    else
    {
        printk("\nDevice does not exist\r\n");
        k_msleep(5000);
    }
    return 0;
}

static int iis3wdb_channel_get(const struct device *dev,
			      enum sensor_channel chan,
			      struct sensor_value *val)
{
    struct iis3wdb_data *dta = dev->data;
    
    switch(chan)
    {
        case SENSOR_CHAN_ACCEL_X:
            val->val1 = dta->x;
        break;
        case SENSOR_CHAN_ACCEL_Y:
            val->val1 = dta->y;

        break;
        case SENSOR_CHAN_ACCEL_Z:
            val->val1 = dta->z;
        break;
    }
}

static int get_acceleration( const struct device *dev)
{
    struct iis3wdb_data *dta = dev->data;
	const struct iis3wdb_config *cfg = dev->config;


    dta->x = ((IIS3WDB_read_reg(dev,ADD_REG_OUT_X_H) << 8) | IIS3WDB_read_reg(dev,ADD_REG_OUT_X_L));
    dta->y = ((IIS3WDB_read_reg(dev,ADD_REG_OUT_Y_H) << 8) | IIS3WDB_read_reg(dev,ADD_REG_OUT_Y_L));
    dta->z = ((IIS3WDB_read_reg(dev,ADD_REG_OUT_Z_H) << 8) | IIS3WDB_read_reg(dev,ADD_REG_OUT_Z_L));

    /* transform X value from two's complement to 16-bit int */
    dta->x = twoComplToInt16(dta->x);
    /* convert X absolute value to mg value */
    dta->x = dta->x * SENS_2G_RANGE_MG_PER_DIGIT;

    /* transform Y value from two's complement to 16-bit int */
    dta->y = twoComplToInt16(dta->y);
    /* convert Y absolute value to mg value */
    dta->y = dta->y * SENS_2G_RANGE_MG_PER_DIGIT;

    /* transform Z value from two's complement to 16-bit int */
    dta->z = twoComplToInt16(dta->z);
    /* convert Z absolute value to mg value */
    dta->z = dta->z * SENS_2G_RANGE_MG_PER_DIGIT;
    return 0;
}

static const struct sensor_driver_api iis3wdb_driver_api = {
    .channel_get = iis3wdb_channel_get,
    .sample_fetch = get_acceleration
};



//Register a node in the deviceTree 

#if DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT) == 0
#warning "IIS3WDB driver enabled without any devices"
#endif


#define IIS3WDB_DEVICE_INIT(inst)                                                                                                               \
        DEVICE_DT_INST_DEFINE (inst, iis3wdb_init, NULL, &iis3wdb_data_##inst, &iis3wdb_config_##inst, POST_KERNEL,                             \
                               CONFIG_SENSOR_INIT_PRIORITY, &iis3wdb_driver_api);

#define IIS3WDB_CONFIG_SPI(inst)						\
	{								\
		.bus_init = SPI_init,				\
		.bus_cfg = { .spi = SPI_DT_SPEC_INST_GET(inst,		\
					SPI_OP(FRAME_SIZE),			\
					0) },				\
	}

#define IIS3WDB_DEFINE_SPI(inst)						\
	static struct iis3wdb_data iis3wdb_data_##inst;			\
	static const struct iis3wdb_config iis3wdb_config_##inst =	\
	IIS3WDB_CONFIG_SPI(inst);				\
	IIS3WDB_DEVICE_INIT(inst)


    DT_INST_FOREACH_STATUS_OKAY(IIS3WDB_DEFINE_SPI)