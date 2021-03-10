#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "pav_analysis.h"
#include "fic_wave.h"

int main(int argc, char *argv[])
{
    float durTrm = 0.010;
    float fm;
    int N;
    int trm;
    float *x;
    short *buffer;
    FILE *fpWave;
    float zcr, amp, pow; //temporary variables to display them on screen and print them into a file
    FILE *out_file;      //output file

    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Empleo: %s inputfile [outputfile]\n", argv[0]);
        return -1;
    }
    if ((fpWave = abre_wave(argv[1], &fm)) == NULL)
    {
        fprintf(stderr, "Error al abrir %s (%s)\n", argv[1], strerror(errno));
        return -1;
    }
    N = durTrm * fm;

    //open input file
    if ((buffer = malloc(N * sizeof(*buffer))) == 0 ||
        (x = malloc(N * sizeof(*x))) == 0)
    {
        fprintf(stderr, "Error al ubicar los vectores (%s)\n", strerror(errno));
        return -1;
    }

    //open output file
    if (argc == 3 && ((out_file = fopen(argv[2], "w")) == NULL))
    {
        fprintf(stderr, "Error al crear %s (%s)\n", argv[2], strerror(errno));
        return -1;
    }
    trm = 0;
    while (lee_wave(buffer, sizeof(*buffer), N, fpWave) == N)
    {
        for (int n = 0; n < N; n++)
            x[n] = buffer[n] / (float)(1 << 15); //2^15

        //store results in variables
        pow = compute_power(x, N);
        amp = compute_am(x, N);
        zcr = compute_zcr(x, N, fm);

        //display on screen
        printf("trm: %d\tPower estimate: %f dB\tAmplitude: %f\tZCR: %f\n", trm, pow,
               amp,
               zcr);

        //if the user has inputted an output file, results are typed
        if (argc == 3)
            fprintf(out_file, "%d\t%f\t%f\t%f\n", trm, pow, amp, zcr);

        trm += 1;
    }

    //close output file
    if (argc == 3)
        fclose(out_file);

    cierra_wave(fpWave);
    free(buffer);
    free(x);

    return 0;
}