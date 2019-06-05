#include <stdio.h>
#include <iostream>

const int num = 383;
const int x = 16;
const int y = 12;

/*
 * creating an array with all the necessary colors as integers
 */

void createColorArray(int ColorArray[num])
{
    int i = 0;
    int R = 255;
    int G = 0;
    int B = 0;
    for(i = num-1; i >= 0; i--)
    {
       ColorArray[i] = R;
       ColorArray[i] = ColorArray[i] << 8;
       ColorArray[i] += G;
       ColorArray[i] = ColorArray[i] << 8;
       ColorArray[i] += B;

       if( R > 128 && B == 0)
       {
           R --;
           if (G != 254)
           {
               G += 2;
           }
           else
           {
               G ++;
           }
       }
       else if (R <= 128 && B != 255)
       {
           R --;
           if (B != 254)
           {
               B += 2;
           }
           else
           {
               B ++;
           }
       }
       else if (B == 255)
       {
           R ++;
           if (G != 1)
           {
               G -= 2;
           }
           else
           {
               G --;
           }
       }
    }
}

/*
 * finding the min and max temperature measured by the pixels of the sensor
 */

void MinMaxMatrix(float arr[16][12], float &min, float &max)
{
    int i = 0;
    int j = 0;
    min = arr[i][j];
    max = arr[i][j];
    for(i = 0; i < 16; i++)
    {
        for(j = 0; j< 12; j++)
        {
            if(min > arr[i][j])
            {
                min = arr[i][j];
            }
            if(max < arr[i][j])
            {
                max = arr[i][j];
            }
        }
    }
}

/*
 * by incrementing the minimal temperature an array with num temperatures is created ranging from min to max, which means each temperature
 */

void createColorCodeArray(float ColorCode[num], float step, int min, int max)
{
    int i = 0;
    ColorCode[i] = min;
    for (i = 1; i < num; i++)
    {
        ColorCode[i] = ColorCode[i-1] + step;

    }
    ColorCode[num-1] = max;
}

/*
 * codifying the temperature values to integers representing the colors
 */
void codifyTemp(float ColorCode[num], int ColorArray[num], int codedArr[16][12], float arr[16][12])
{
    float minDiff = 999.0;
    int i = 0;
    int j = 0;
    int colornumber = num-1;

    for(i = 0; i < x; i++)
    {
        for(j = 0; j < y; j++)
        {
            minDiff = 999.0;
            do
            {
                if(minDiff > abs((arr[i][j] - ColorCode[colornumber])))
                {
                    minDiff = abs((arr[i][j] - ColorCode[colornumber]));
                    codedArr[i][j] = ColorArray[colornumber];
                }
                colornumber --;
            }
            while(colornumber >= 0);
            colornumber = num-1;
        }
    }
}

void tempToRGB(float arr[16][12], int codedArr[16][12])
{
    /*
     * Done!
     *
     * Create a ColorArray with the numbers correspending to colors
     * Find the min and max from arr and assign them the warmest and coldest color
     * divide the difference between the min and max to the number of elements in ColorArray to get step
     * use step to increment upon the min value and create an array ColorCode from min to ~max with the same number of elements as ColorArray
     * search the closest element in ColorArray to each element in arr and use the color in ColorArray with the corresponding index to codify the temperature represented by the arr element
     * output: a matrix containing the codified temperatures with the same dimensions as arr
     *
     * hint: for the ColorArray values, check https://stackoverflow.com/questions/4615029/rgb-range-for-cold-and-warm-colors
     */

    int ColorArray[num];
    float ColorCode[num];
    float min = 0;
    float max = 0;
    float step;
    createColorArray(ColorArray);
    MinMaxMatrix(arr,min,max);

    step = (max - min) / num;

    createColorCodeArray(ColorCode,step,min,max);
    codifyTemp(ColorCode,ColorArray,codedArr,arr);
}



