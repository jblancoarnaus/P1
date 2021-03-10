#include <stdio.h>
#include "fic_wave.h"

FILE *abre_wave(const char *ficWave, float *fm)
{
    FILE *fpWave;
    unsigned char fm_str[10];
    unsigned int fm_int;
    if ((fpWave = fopen(ficWave, "r")) == NULL)
        return NULL;

    if (fseek(fpWave, 44, SEEK_SET) < 0)
        return NULL;

    //Go to byte 24 (sampling frequency info)
    fseek(fpWave, 24, SEEK_SET);
    //read 4 bytes and store them into fm_str
    fread(fm_str, 1, 4, fpWave);
    //rearrange 4 bytes into a number taking into account the order (little endian)
    fm_int = fm_str[0] + (fm_str[1]<<8) + (fm_str[2]<<16) + (fm_str[3]<<24);
    *fm = fm_int;
    return fpWave;
}

size_t lee_wave(void *x, size_t size, size_t nmemb, FILE *fpWave)
{
    return fread(x, size, nmemb, fpWave);
}

void cierra_wave(FILE *fpWave)
{
    fclose(fpWave);
}