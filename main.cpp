#include "I2C_Driver.h"
#include "calculation.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <QApplication>
#include <myrect.h>
#include <temptogb.h>

void delay(float number_of_miliseconds);

int main (int argc, char *argv [])
{
    QApplication app(argc, argv);

    SomeWidget widget;
    widget.show();

    float TempArray[16][12];
    int ColorArray[16][12];

   uint16_t status_register;
   uint16_t resolution_register;
   uint8_t slave = 0x33;
   static uint16_t EepromData[832];

   static uint16_t RamData[448];

   int x = MLX90641_I2CRead(slave,0x2400,832,EepromData);     // extract EEprom and put it in EepromData


   int subp = 0;
   int i=448;
  /* for (i; i<640;i++)
   {
           printf("Element %d is",i);
           printf(" %d \n",EepromData[i]);
   }*/
   restore_Eeprom(EepromData);
    x = MLX90641_I2CRead(slave,0x800D,1,&resolution_register);
    resolution_register = (resolution_register&3072)/pow(2,10);
   printf ("\n\n==============================\n=======Ram starts here:=======\n==============================\n\n");
   int br = 0;
   int l = 0;
   do
   {
       x = MLX90641_I2CRead(slave,0x8000,1,&status_register);

       if((status_register&8) == 8)
       {
           printf("\n statusreg should be >=8 ***88888*** it is: %d\n", status_register);
            x = MLX90641_I2CWrite(slave,0x8000,status_register&65527);   //65527 = 0b1111111111110111
            x = MLX90641_I2CRead(slave,0x0400,448,RamData);    // extract Ram and put it in RamData
            l = (status_register&1);
            if((status_register&1) == 0)
            {
                printf("\n********Subpage = %d********\n",status_register&1);
                subp = 0;
            }
            else
            {
                printf("\n********Subpage = %d********\n*********%d*********\n",status_register&1,status_register);
                subp = 1;
            }
            x = instance (subp,EepromData,RamData,resolution_register, TempArray);
            tempToRGB(TempArray, ColorArray);
            for (int k = 0; k<16;k++)
            {
                for (int l = 0; l<12;l++)
                {
                    widget.colorArray[k][l] = ColorArray[k][l];
                }
            }
//            widget.show();
            app.processEvents();
            delay(0.00040);
           br++;
       }
   }
   while(br != 10);

   x = MLX90641_I2CEnd();
   printf("Press ENTER key to Continue\n");
   getchar();

//    return app.exec();
}

void delay(float number_of_miliseconds)
{
    // Converting time into milli_seconds
    //int milli_seconds = number_of_seconds;

    // Stroing start time
    clock_t start_time = clock();

    // looping till required time is not acheived
    while (clock() < start_time + number_of_miliseconds)
    ;
}
