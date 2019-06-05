/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */


#ifndef CALCULATION_H
#define CALCULATION_H

//#include <stdint.h>

/*struct MLX90641
{
  int16_t Vdd25;
  int16_t Kvdd;
  float Vdd;
  float Ta;
  float offset;
  float sensitivity;
  float Kta;
  float Kv;
  float gain;
  float KsTa;
  float KsTo;
  float emissivity;
  float sensitivityacp;
  float offset_CP;
  float Kta_CP
  float TGC
  float resolution
}*/

float restore_VDD(uint8_t Res, uint16_t Ram[], uint16_t EEprom[]);
float restore_Ta(uint16_t Ram[],uint16_t EEprom[]);
float restore_offset(int subp, uint16_t EEprom[], int iteration);
double restore_sensitivity(uint16_t p, uint16_t EEprom[], int iteration);
float restore_Kta(uint16_t EEprom[], int iteration);
float restore_Kv(uint16_t EEprom[], int iteration);
float restore_gain(uint16_t EEprom[]);
float restore_KsTa(uint16_t EEprom[]);
float restore_KsTo(uint16_t EEprom[]);
float restore_emissivity(uint16_t EEprom[]);
double restore_sensitivityacp(uint16_t EEprom[]);
float restore_offset_CP (uint16_t EEprom[]);
float restore_Kta_CP (uint16_t EEprom[]);
float restore_TGC(uint16_t EEprom[]);
uint8_t restore_resolution_control(uint16_t EEprom[]);
void restore_Eeprom(uint16_t EEprom[]);
void newarray(uint16_t Ram[],float RamProper[][16], int subp);
int instance (int subp, uint16_t EepromData[], uint16_t RamData[], uint16_t resolution_control, float TempArray[16][12]);

#endif
