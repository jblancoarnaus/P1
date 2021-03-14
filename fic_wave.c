#include <stdio.h>
#include "fic_wave.h"
#include <errno.h>

FILE *abre_wave(const char *ficWave, float *fm, unsigned int *confOK)
{
    FILE *fpWave;
    unsigned char fm_str[10], temp[10];
    unsigned int fm_int, encoding, channels, bits;
    if ((fpWave = fopen(ficWave, "r")) == NULL)
        return NULL;

    if (fseek(fpWave, 44, SEEK_SET) < 0)
        return NULL;

    //Go to byte 20 (check if its linear PCM encoding)
    fseek(fpWave, 20, SEEK_SET);
    fread(temp, 1, 2, fpWave);
    //rearrange the 2 bytes into a number taking into account the order (little endian)
    encoding = (temp[0] + (temp[1] << 8)); //linear PCM encoding == 1

    //obtain the number of channels (byte 22)
    fread(temp, 1, 2, fpWave);
    channels = (temp[0] + (temp[1] << 8)); //convert to little endian

    //obtain sampling frequency info (byte 24)
    fread(fm_str, 1, 4, fpWave);
    fm_int = fm_str[0] + (fm_str[1] << 8) + (fm_str[2] << 16) + (fm_str[3] << 24); //convert to little endian
    *fm = fm_int;

    //Go to byte 34 (check if it uses 16 bits per sample)
    fseek(fpWave, 6, SEEK_CUR); //instead of setting pointer to the beginning of the FILE (SEEK_SET), uses the current position (28)
    fread(temp, 1, 2, fpWave);
    bits = (temp[0] + (temp[1] << 8)); //convert to little endian

    printf("****Audio properties****\nEncoding: %d\tNumber of channels: %d\tSampling frequency: %d\tBits/sample: %d\n************************\n", encoding, channels, fm_int, bits);
    //Display error if encoding isnt 16Lin or has more than 2 channels
    if (bits != 16 || encoding != 1 || (channels != 1 ))
    {
        *confOK = 0;
        return NULL;
    }
    fseek(fpWave, 8, SEEK_CUR); //Set pointer to byte 44 (data)
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