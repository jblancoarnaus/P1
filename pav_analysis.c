#include <math.h>
#include "pav_analysis.h"
#include <stdio.h>

//Computes the power of x in dB
//  x: input signal
//  N: # of samples
float compute_power(const float *x, unsigned int N)
{
    float power = 0;
    int i = 0;

    for (i = 0; i < N; i++)
    {
        power = power + x[i] * x[i]; //power = power + x[i]^2
    }
    power = power * (1.0 / N);
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
