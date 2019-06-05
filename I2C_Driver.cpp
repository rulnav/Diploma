 //#include <iostream>
#include <bcm2835.h>
#include "I2C_Driver.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
int init = 0;

void MLX90641_I2CInit()
{
    
}

int MLX90641_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{
    if(!init){
        bcm2835_init();
    	bcm2835_i2c_begin();
	bcm2835_i2c_set_baudrate(400000);
	init = 1;
    }

    int result;

    char cmd[2] = {(char)(startAddress >> 8), (char)(startAddress & 0xFF)};
    //printf("%s %s\n", cmd[0],cmd[1]);

    bcm2835_i2c_setSlaveAddress(slaveAddr);

    char buf[1664];
    uint16_t *p = data;

    result = bcm2835_i2c_write_read_rs(cmd, 2, buf, nMemAddressRead*2);
    if (result<0){
	printf("ERROR: %s\n", strerror(errno));
    }
    for(int count = 0; count < nMemAddressRead; count++){
	int i = count << 1;
    	*p++ = ((uint16_t)buf[i] << 8) | buf[i+1];
        //printf("show wtf %X\n",*p);
    }
    return 0;
} 

void MLX90641_I2CFreqSet(int freq)
{
}

int MLX90641_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)
{
    int result;
    char cmd[4] = {(char)(writeAddress >> 8), (char)(writeAddress & 0x00FF), (char)(data >> 8), (char)(data & 0x00FF)};
    result = bcm2835_i2c_write(cmd, 4);
    return 0;
}
int MLX90641_I2CEnd()
{
   bcm2835_i2c_end();
   return 0;
}
