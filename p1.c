#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "pav_analysis.h"
#include "fic_wave.h"

int main(int argc, char *argv[])
{
    float durTrm = 0.010;       //window duration in s
    float durDesp = 0.0;      //window displacement in s
    float fm;
    int N;
    int trm;
    float *x;
    short *buffer;
    FILE *fpWave;
    int Ndesp_bits;            //Displacement in bits
    unsigned int confOK = 1;   //==1 if input file is encoded with PCM Lin16 and 1 channel, ==0 otherwise
    unsigned int h_window = 0; //==1 hamming window, else rectangular
    float zcr, amp, pow;       //temporary variables to display them on screen and print them into a file
    FILE *out_file;            //output file

    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Empleo: %s inputfile [outputfile]\n", argv[0]);
        return -1;
    }

    //We cant use errno to show an error caused by using the wrong audio settings (last if condition)
    if (((fpWave = abre_wave(argv[1], &fm, &confOK)) == NULL) && confOK != 0)
    {
        fprintf(stderr, "Error al abrir %s (%s)\n", argv[1], strerror(errno));
        return -1;
    }
    else if (confOK == 0)
    {
        fprintf(stderr, "Error al procesar %s (Fichero ha de tener 1 canal y configuración Lin16)\n", argv[1]);
        return -1;
    }

    N = durTrm * fm;
    Ndesp_bits = durDesp * fm * 16 * (1.0 / 8); //convert to bits
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
        {
            x[n] = buffer[n] / (float)(1 << 15); 
        }

        //move pointer Ndesp_bits backwards (hamming window overlaps)
        fseek(fpWave, (int)-Ndesp_bits, SEEK_CUR);

        //store results in variables
        pow = compute_power(x, N, h_window);
        amp = compute_am(x, N);
        zcr = compute_zcr(x, N, fm);

        //display on screen
        printf("trm: %d\tPower estimate: %f dB\tAmplitude: %f\tZCR: %f\n", trm, pow,
               amp, zcr);
        //if the user has inputted an output file, results are typed in
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