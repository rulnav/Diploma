#ifndef I2C_Driver_H
#define I2C_Driver_H

#include <stdint.h>

    void MLX90641_I2CInit(void);
    int MLX90641_I2CRead(uint8_t slaveAddr,uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data);
    int MLX90641_I2CWrite(uint8_t slaveAddr,uint16_t writeAddress, uint16_t data);
    void MLX90641_I2CFreqSet(int freq);
    int MLX90641_I2CEnd(void);
#endif
