#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "I2C_Driver.h"
#include "calculation.h"

int16_t Vdd25;
int16_t Kvdd;


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
float restore_Kv_CP (uint16_t EEprom[]);
void printarray(uint16_t Ram[]);

int instance (int subp, uint16_t EepromData[], uint16_t RamData[], uint16_t resolution_register, float TempArray[16][12])
{
  FILE *fp;
  FILE *fp2;
  int j = 0;
  fp = fopen ("/home/pi/programming/C-code/Diploma/file.csv","w");
   fp2 = fopen ("/home/pi/programming/C-code/Diploma/offset.txt","w");
  fprintf(fp,"Subpage is %d \nThis is Eeprom.\n",subp);
   for (j = 0; j<832;j++)
   {

       //fputc(EepromData[i],fp);
       fprintf(fp,"%d ",EepromData[j]);
   }
  fprintf(fp,"\nThis is Ram.\n");
   /*for (j = 0;j<447;j++)
   {
       //fputc(RamData[i],fp);
       fprintf(fp,"%d ",RamData[j]);
   }*/


  j = 0;
 // int i = 0;
 //   printf("\n==============HERE==============\n");

  //printf("\n==============HERE==============\n");
  float RamProper[12][16];
  uint8_t res = restore_resolution_control(EepromData);
  res = pow(2,res)/pow(2,resolution_register);
  //res= 2;
  //printf("\n==============HERE==============\n");
  newarray(RamData, RamProper, subp);
  //printf("\n==============HERE is last element %f==============\n",RamProper[11][15]);
  float Vdd;
  float Ta;
  float Kgain;
  float pixg;
  float cppixg;
  float Kta;
  float Kta_CP;
  float Kv;
  float Kv_CP;
  float pixosp;
  float cppixos;
  float emissivity;
  float TGC;
  float Vir;

  int row = 0;
  int col = 0;

  uint8_t pixelrow;
  float a;
  float a_cp;
  float KsTa;
  double acomp;
  float Tak4;
  float Trk4;
  float Ta_r4;
  float KsTo;
  double Sx;
  float To;
  float offset;
  float offset_CP;
  int i = 0;
  int col_true = 15;
  //printf("\n==============HERE1 %d==============\n",EepromData[51]);

  for (row = 0; row<12;row++)
  {
      col_true = 15;
      fprintf(fp2, "\n");
      for (col = 0; col<16;col++)
      {
            //printf("\n==============HERE2==============\n");

         //printf("\n==============HERE3  %f ==============\n", Vdd);
            Vdd = restore_VDD(res,RamData,EepromData);
            //Vdd = restore_VDD(res,RamData,EepromData);
            Ta = restore_Ta(RamData,EepromData);
            if (RamData[394]>32767)
            {
                Kgain = restore_gain(EepromData)/(RamData[394] - 65536);
            }
            else
            {
                Kgain = restore_gain(EepromData)/(RamData[394]);
            }
            pixg = RamProper[row][col]*Kgain;

            if (RamData[392] > 32767)
            {
                cppixg = (RamData[392] - 65536)*Kgain;
            }
            else
            {
                cppixg = RamData[392]*Kgain;
            }

            Kta = restore_Kta(EepromData, i);
            Kta_CP = restore_Kta_CP(EepromData);
            Kv = restore_Kv(EepromData, i);
            Kv_CP = restore_Kv_CP(EepromData);



            offset = restore_offset(subp,EepromData, i);
            fprintf(fp2,"%f ;",offset);
            offset_CP = restore_offset_CP(EepromData);
            pixosp = pixg - offset * (1 + Kta * (Ta - 25)) * (1 + Kv * (Vdd - 3.3));
            //printf("\n==============HERE4==============\n");
            cppixos = cppixg - offset_CP * (1 + Kta_CP * (Ta - 25))* (1 + Kv_CP * (Vdd -3.3));
            //printf("\n==============HERE5==============\n");
            emissivity = restore_emissivity(EepromData);
            TGC = restore_TGC(EepromData);
            Vir = pixosp/emissivity - TGC*cppixos;



            pixelrow = ((16*(row+1-1)+col_true+1)-1)/32 + 1;
            a = restore_sensitivity(pixelrow,EepromData, i );
            a_cp = restore_sensitivityacp(EepromData);
            KsTa = restore_KsTa(EepromData);
            acomp = (a - TGC*a_cp)*(1+KsTa*(Ta - 25));

            Tak4 = pow((Ta + 273.15),4);
            Trk4 = pow((Ta - 5 +273.15),4);
            Ta_r4 = Trk4 - (Trk4 - Tak4)/emissivity;
            KsTo = restore_KsTo(EepromData);
            Sx = KsTo * sqrt(sqrt(pow(acomp,3)*Vir + pow(acomp,4) * Ta_r4));
            //Sx = 0;
            To = sqrt(sqrt(Vir/(acomp*(1-KsTo*273.15)+Sx)+Ta_r4)) - 273.15;
            //printf("\n %f C",To);
            /*if (i == 0)
            {
                printf ("\n ====old==== \n");
                for (int m = 0; m<12;m++)
                {
                    for (int n = 0; n<16;n++)
                    {
                        printf ("%.0f ",RamProper[m][n] );
                    }
                    printf("\n");
                }
                printf("\n==============HERE==============\n");
                printf("\n==============HERE emissivity and TGC, %f and %f==============\n", emissivity,TGC);
                printf("\n==============HERE KsTa and To, %f and %f==============\n", KsTa,To);
                printf("\n==============HERE a and a_CP, %.12f and %.12f==============\n", a, a_cp);
                printf("\n==============HERE restore_offset and Sx, %f and %f==============\n", offset, Sx);
                printf("\n==============HERE acomp and KsTo, %.12f and %f==============\n", acomp, KsTo);
                printf("\n==============HERE Vdd and Kgain, %f and %f==============\n", Vdd, Kgain);
                printf("\n==============HERE emissivity and TGC, %f and %f==============\n", emissivity, TGC);
                printf("\n==============HERE Kv and Kv_CP, %f and %f==============\n", Kv, Kv_CP);
                printf("\n==============HERE Kta and Kta_CP, %f and %f==============\n", Kta, Kta_CP);
                printf("\n==============HERE pixg and cppixg, %f and %f==============\n", pixg, cppixg);
                printf("\n==============HERE pixosp and cppixosp, %f and %f==============\n", pixosp, cppixos);
                printf("\n==============HERE VIR and Ta_r4, %f and %f==============\n", Vir, Ta_r4);
                printf("\n==============HERE Ta and offset_CP, %f and %f==============\n", Ta, offset_CP);
                printf("\n");
            }*/
            //if (i == 120) printf("\n==============HERE Kv and Kv_CP, %f and %f==============\n", Kv, Kv_CP);

            fprintf (fp,"%.0f ",RamProper[row][col]);     //file debug

            RamProper[row][col] = To;

            i++;
            //printf("col_true %d, i, %d, row %d, col %d, pixelrow %d \n", col_true, i, row, col, pixelrow);
            col_true --;

            //if(i ==5) break;
        }
        //if(i ==5) break;
  }
  //x = MLX90641_I2CEnd();
  printf("\n==============HERE==============\n");
  printf("\n==============HERE emissivity and TGC, %f and %f==============\n", emissivity,TGC);
  printf("\n==============HERE KsTa and To, %f and %f==============\n", KsTa,To);
  printf("\n==============HERE a and a_CP, %.12f and %.12f==============\n", a, a_cp);
  printf("\n==============HERE restore_offset and Sx, %f and %f==============\n", offset, Sx);
  printf("\n==============HERE acomp and KsTo, %.12f and %f==============\n", acomp, KsTo);
  printf("\n==============HERE Vdd and Kgain, %f and %f==============\n", Vdd, Kgain);
  printf("\n==============HERE emissivity and TGC, %f and %f==============\n", emissivity, TGC);
  printf("\n==============HERE Kv and Kv_CP, %f and %f==============\n", Kv, Kv_CP);
  printf("\n==============HERE Kta and Kta_CP, %f and %f==============\n", Kta, Kta_CP);
  printf("\n==============HERE pixg and cppixg, %f and %f==============\n", pixg, cppixg);
  printf("\n==============HERE pixosp and cppixosp, %f and %f==============\n", pixosp, cppixos);
  printf("\n==============HERE VIR and Ta_r4, %f and %f==============\n", Vir, Ta_r4);
  printf("\n==============HERE Ta and offset_CP, %f and %f==============\n", Ta, offset_CP);
  printf("\n");
  printf ("\n ====new==== \n");
  fprintf(fp,"\nThis is the rest of the Ram\n");
  for (j = 384;j<448;j++)
   {
       //fputc(RamData[i],fp);
       fprintf(fp,"%d ",RamData[j]);
   }
  fprintf(fp,"\nThis is Results To\n");
  for (int m = 0; m<12;m++)
  {
      for (int n = 0; n<16;n++)
      {
          printf ("%f ",RamProper[m][n] );
          //fputc(RamProper[m][n],fp);
          fprintf(fp,"%f ",RamProper[m][n]);
          TempArray[n][m] = RamProper[m][n];
      }
      printf("\n");
  }
  //printf ("\n ====old==== \n");
  //printarray(RamData);
  fclose(fp);
  fclose(fp2);
  return 1;
}


void restore_Eeprom(uint16_t EEprom[])                                  // sets the four MSBs in a code to 0
{
  int i = 0;
  for (i;i<832;i++)
  {

     EEprom[i] = EEprom[i]&0x07FF;
  }
}
void printarray(uint16_t Ram[])
{
    int i = 0;
    int j = 0;
    for (i;i<384;i++)
    {

        for (j = 0; j<16; j++)
        {

            if(Ram[i+j]>32767)
            {
                printf("%d ", Ram[i+j] - 65536);
            }
            //printf("%d ", i+j);
        }
        i+= j-1;
        printf("\n");
    }
}



void newarray(uint16_t Ram[],float RamProper[][16], int subp)
{
  int row = 0;
  int col = 0;
  int br = 0;
  int i = 0;
  if (subp == 0)
  {
      br = 0;
  }
  else
  {
    br = 32;
  }
  //for (i;i<384;i++)
//{
    //printf("\n==============HERE==============\n");
    while (row <12)
    {
      if(i>= br && i<br+16)
      {
        col = i - br;
        //printf("\nHERE col is %d and i is %d and br is %d==============\n",col, i, br);
        if(Ram[i]>32767)
        {
            RamProper[row][col] = Ram[i] - 65536;
        }
        else
        {
            RamProper[row][col] = Ram[i];
        }
        if (col == 15)
        {
           row++;
           //printf("\n==============HERE row is %d",row);
           if(row%2 == 1)
           {
                br+=16;
           }
           else
           {
                br+=48;
           }
           //printf(" and HERE br is %d==============\n",br);
        }
        //break;
      }
      i++;
      //RamProper[row][col] = Ram[i];
    }
  //}
}
float restore_VDD(uint8_t Res, uint16_t Ram[], uint16_t EEprom[])
{
  float Vdd = 0;
  int ram = Ram[426];

  Kvdd = (EEprom[39]&0x07FF);
  Vdd25 = EEprom[38]&0x7FF;

  if(Kvdd>1023)
  {
    Kvdd = Kvdd - 2048;
  }
  Kvdd = Kvdd*pow(2,5);

  if(ram>32767)
  {
    ram = ram - 65536;
  }
//printf(" and HERE 1AA is %d==============\n",ram);
  if(Vdd25>1023)
  {
    Vdd25 = Vdd25 - 2048;
  }
  Vdd25 = Vdd25*pow(2,5);
  Vdd = ((Res*ram - Vdd25)/Kvdd)+3.3;

  return Vdd;
}

float restore_Ta(uint16_t Ram[], uint16_t EEprom[])
{
  /*int ram = Ram[p];
  if(ram>32767)
  {
    ram = ram - 65536;
  }*/
   printf("\n=====!!!======HERE Vdd25 and Kvdd, %d and %d==============\n", Vdd25, Kvdd);
  int16_t Kvptat_temp = EEprom[43]&0x07FF;
  float Kvptat;
  int16_t Ktptat_temp = EEprom[42]&0x07FF;
  float Ktptat;
  int16_t dV_temp = Ram[426];
  float dV;
  uint16_t Vptat25 = 32*(EEprom[40]&0x07FF)+(EEprom[41]&0x07FF);
  int Vptat = Ram[416];
  int Vee = Ram[384];
  float Alphaptat = (EEprom[44]&0x07FF)/pow(2,7);
  float Vptatart;
  float Ta;

  if(Kvptat_temp>1023)
  {
    Kvptat_temp = Kvptat_temp-2048;
  }
  Kvptat = Kvptat_temp/pow(2,12);

  if(Ktptat_temp>1023)
  {
    Ktptat_temp = Ktptat_temp - 2048;
  }
  Ktptat = Ktptat_temp/pow(2,3);

  if (Vptat>32767)
  {
    Vptat = Vptat - 65536;
  }

  if (Vee>32767)
  {
    Vee = Vee - 65536;
  }
  if (dV_temp > 32767)
  {
      dV_temp = dV_temp - 65536;
  }
  dV = (dV_temp - Vdd25)/Kvdd;

  Vptatart = (Vptat/(Vptat*Alphaptat + Vee))*pow(2,18);

  Ta = (Vptatart/(1+Kvptat*dV)-Vptat25)/Ktptat + 25;
  printf("\n=====!!!======HERE Ta is %f and Vptatart is %f and Vee is %d and Vptat is %d and dV is %f and Kvptat is %f and Ktptat is %f and Vptat25 is %d and Alphaptat is %f==============\n", Ta, Vptatart, Vee, Vptat,dV,Kvptat,Ktptat,Vptat25,Alphaptat);
  return Ta;
}

float restore_offset(int subp, uint16_t EEprom[], int iteration)
{
  float pix;
  int offsetavarage = 32 * (EEprom[17]&0x07FF) + (EEprom[18]&0x07FF);
  int16_t offsetsp;
  float offsetscale = (EEprom[16]&0x07E0)/pow(2,5);

  if (offsetavarage>32767)
  {
    offsetavarage = offsetavarage - 65536;
  }

  if(subp == 0)
  {
    offsetsp = EEprom[64 + iteration] & 0x07FF;
  }
  else
  {
    offsetsp = EEprom[640 + iteration] & 0x07FF;
  }

  if (offsetsp >1023)
  {
    offsetsp = offsetsp - 2048;
  }

  pix = offsetavarage + offsetsp * pow(2,offsetscale);
  printf("\n=====!!!!======-HERE,  pix %f and offsetscale is %.9f and offsetavarage (Pix_os_r1) is %d and %d===========\n", pix, offsetscale, offsetavarage, offsetsp);
  return pix;
}

double restore_sensitivity(uint16_t p, uint16_t EEprom[], int iteration)
{
  int aref;
  double a;
  //int a2;
  float ascale;
  uint16_t apix = EEprom[256+iteration]&0x07FF;

  if (p == 1)
  {
    aref = EEprom[28];
    ascale = (EEprom[25]&0x07E0)/pow(2,5) + 20;
  }
  if (p == 2)
  {
    aref = EEprom[29];
    ascale = (EEprom[25]&0x001F) + 20;
  }
  if (p == 3)
  {
    aref = EEprom[30];
    ascale = (EEprom[26]&0x07E0)/pow(2,5) + 20;
  }
  if (p == 4)
  {
    aref = EEprom[31];
    ascale = (EEprom[26]&0x001F) + 20;
  }
  if (p == 5)
  {
    aref = EEprom[32];
    ascale = (EEprom[27]&0x07E0)/pow(2,5) + 20;
  }
  if (p == 6)
  {
    aref = EEprom[33];
    ascale = (EEprom[27]&0x001F) + 20;
  }

  a = (aref&0x07FF)/pow(2,ascale);
  a = apix*a/(pow(2,11)-1);
  //a2 = apix*aref/(pow(2,11)-1);
  printf("\n=====!!!!======-HERE,  a (sensitivity) is %.9f and p (row) is %d and apix is %d and ascale (scale_row) is %f and aref (row_max) is %.9f===========\n", a, p, apix, ascale, (aref&0x07FF)/pow(2,ascale));
  return a;
}

float restore_Kta(uint16_t EEprom[], int iteration)
{
  float Kta;
  float Ktaee = (EEprom[448 + iteration]&0x07E0)/pow(2,5);
  int16_t Ktaaverage = EEprom[21]&0x07FF;
  float Ktascale1 = (EEprom[22]&0x07E0)/pow(2,5);
  uint16_t Ktascale2 = EEprom[22]&0x001F;

  if (Ktaee>31)
  {
    Ktaee = Ktaee - 64;
  }
  if (Ktaaverage>1023)
  {
    Ktaaverage = Ktaaverage - 2048;
  }

  Kta = (Ktaee * pow(2,Ktascale2) + Ktaaverage)/pow(2,Ktascale1);
  printf("\n=====!!!!======-HERE,  Ktaee %f and Ktaaverage is %d and %d[%d] and Kta %f and Ktascale1 %f and Ktascale2 %d==============\n", Ktaee, Ktaaverage, EEprom[448+iteration], 448+iteration,Kta,Ktascale1,Ktascale2);
  //if (iteration %32 == 0) printf ("\n");
  //printf ("%d ", EEprom[448+iteration]);
  //printf ("%.6f ",Kta);
  return Kta;
}

float restore_Kv(uint16_t EEprom[], int iteration)
{
  float Kv;
  int16_t Kvee = EEprom[448 + iteration]&0x001F;
  int16_t Kvaverage = EEprom[23]&0x07FF;
  float Kvscale1 = (EEprom[24]&0x07E0)/pow(2,5);
  uint16_t Kvscale2 = EEprom[24]&0x001F;

  if (Kvee>15)
  {
    Kvee = Kvee - 32;
  }
  if (Kvaverage>1023)
  {
    Kvaverage = Kvaverage - 2048;
  }

  Kv = (Kvee * pow(2,Kvscale2) + Kvaverage)/pow(2,Kvscale1);
  printf("\n=====!!!!======-HERE,  Kvee %d and Kvaverage is %d and %d[%d] and Kvscale1 %f, Kvscale2 %d, Kv is %f==============\n", Kvee, Kvaverage, EEprom[448+iteration], 448+iteration,Kvscale1,Kvscale2, Kv);
  //if (iteration %32 == 0) printf ("\n");
  //printf ("%d ", EEprom[448+iteration]);
  //printf ("%.5f ",Kv);

  return Kv;
}

float restore_gain(uint16_t EEprom[])
{
  float Gain = 32 * (EEprom[36]&0x07FF) + (EEprom[37]&0x07FF);

  printf("=========Gain (GainMeasRef) is: %f", Gain);

  return Gain;
}

float restore_KsTa(uint16_t EEprom[])
{
  int16_t KsTa = EEprom[34]&0x07FF;

  if(KsTa>1023)
  {
    KsTa = KsTa - 2048;
  }
  printf ("\nKSTA : %f\n",KsTa/pow(2,15));
  return (KsTa/pow(2,15));
}

float restore_KsTo(uint16_t EEprom[])
{
  float KsTo3;
  int KsTo;
  uint16_t KsToscale = EEprom[52]&0x07FF;
  KsTo = (EEprom[55]&0x07FF);
  if(KsTo>1023)
  {
    KsTo = KsTo - 2048;
  }
  KsTo3 = KsTo/pow(2,KsToscale);
  return KsTo3;
}

float restore_emissivity(uint16_t EEprom[])
{
  int16_t emissivity = EEprom[35]&0x07FF;
  if(emissivity>1023)
  {
    emissivity = emissivity - 2048;
  }
  return  emissivity/pow(2,9);
}

double restore_sensitivityacp(uint16_t EEprom[])
{
  uint16_t ascale_CP = EEprom[46]&0x07FF;

  double acp = (EEprom[45]&0x07FF)/pow(2,ascale_CP);
  printf("\n===================%d======================%d======================\n",ascale_CP,EEprom[45]&0x07FF);
  return acp ;
}

float restore_offset_CP (uint16_t EEprom[]) // CP - compensation pixel
{
  float off_CP = 32*(EEprom[47]&0x07FF) + (EEprom[48]&0x07FF);
  if (off_CP>32767)
  {
    off_CP = off_CP - 65536;
  }
  return off_CP;
}

float restore_Kv_CP (uint16_t EEprom[])
{
  float KVcp;
  int16_t KVcp_EE = EEprom[50]&0x003F;
  float KVscale = (EEprom[50]&0x07C0)/pow(2,6);
  if(KVcp_EE>31)
  {
    KVcp_EE = KVcp_EE - 64;
  }
  return KVcp_EE/pow(2,KVscale);
}

float restore_Kta_CP (uint16_t EEprom[])
{
  float Kta_cp;
  float Ktascale = (EEprom[49]&0x07C0)/pow(2,6);
  int16_t Ktacp_EE = EEprom[49]&0x003F;
  if(Ktacp_EE>31)
  {
    Ktacp_EE = Ktacp_EE - 64;
  }
  return Ktacp_EE/pow(2,Ktascale);
}

float restore_TGC(uint16_t EEprom[])
{
  float TGC;
  int TGCee = EEprom[51] & 0x01FF;
  if (TGCee>255)
  {
    TGCee = TGCee - 512;
  }
  TGC = TGCee/pow(2,6);
  return TGC;
}

uint8_t restore_resolution_control(uint16_t EEprom[])
{
  uint8_t resolution = (EEprom[51] & 0x0600)/pow(2,9);
  return resolution;
}
