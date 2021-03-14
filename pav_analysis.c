#include <math.h>
#include "pav_analysis.h"
#include <stdio.h>

//Computes the power of x in dB using a hamming window
//  x: input signal
//  N: # of samples
//  h_window: ==1 hamming window, else rectangle window
float compute_power(const float *x, unsigned int N, unsigned int h_window)
{

    float alpha;   //hamming window alpha parameter. ==1 rectangular window
    float h_i = 0; //i sample of the hamming window
    float power_num = 0;
    float power_den = 0;
    float power = 0;
    int i = 0;
    //set alpha
    if (h_window == 1){
        alpha = 0.54;
    }
    else{
        alpha = 1;
    }
    for (i = 0; i < N; i++)
    {
        h_i = alpha + (1 - alpha) * cos((2 * M_PI / N) * i); //obtain i sample of hamming window
        power_num = power_num + (x[i] * h_i) * (x[i] * h_i); //(x[i]*h_i)^2
        power_den = power_den + h_i * h_i;                   //h_i^2; sums N if h_i==1 (alpha==1)
    }
    power = power_num / power_den;
    //convert to dB
    power = 10 * log10(power);
    return power;
}

//Computes the average amplitude (normalised) of x
//  x: input signal
//  N: # of samples
float compute_am(const float *x, unsigned int N)
{
    float amplitude = 0;
    int i = 0;

    for (i = 0; i < N; i++)
    {
        //check if it's negative to sum the absolute value
        if (x[i] < 0)
            amplitude = amplitude - x[i];
        else
            amplitude = amplitude + x[i];
    }
    amplitude = amplitude * (1.0 / N);
    return amplitude;
}

//Computes the zero crossing rate of x
//  x: input signal
//  N: # of samples
float compute_zcr(const float *x, unsigned int N, float fm)
{
    float zcr = 0;
    int i = 0;

    for (i = 1; i < N; i++)
    {
        //check if x went from a positive sample to a negative one and vice versa
        if ((x[i - 1] * x[i] < 0))
            zcr = zcr + 1;
    }
    //obtain ratio
    zcr = zcr / (N - 1);
    //normalise value (Nyquist's frequency)
    zcr = (fm / 2) * zcr;
    return zcr;
}
