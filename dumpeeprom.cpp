#include <iostream>
#include "I2C_Driver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

using namespace std;
int main (int argc, char *argv [])
{

  uint16_t eeValue;
  uint8_t slave = 0x33;
  
  MLX90641_I2CRead(slave,0x240C,1,&eeValue);
  printf("\n%X",eeValue);
}
