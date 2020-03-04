
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/main.h"

const bool SHOW_RASTER = false;

int main(int argc, char *argv[])
{
    switch (argc)
    {
    case 3:
        mainDecrypt(argv[2]);
        break;
    case 4:
        mainEncrypt(argv[2], argv[3]);
        break;
    default:
        printf("Wrong parameters, type:\nsteg e input.ppm output.ppm\nsteg d input.ppm");
        return -1;
    }
}

int mainEncrypt(char* fileName, char *targetName)
{
    // Open ppm
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Cannot open ppm file.");
        return -1;
    }
    struct PPM *ppm = getPPM(fp);
    if ((int)ppm == -1)
        return -1;

    // Get key
    printf("\nKey?\n> ");
    char response[100];
    gets(response);
    int key = atoi(response);

    // Get message
    printf("\nMessage?\n> ");
    gets(response);

    // Encode
    struct PPM *encoded = encode(ppm, response, 1000, key);
    printf("\nEncoded.\n");

    // Write encoded ppm
    FILE *tp = fopen(targetName, "w");
    if (tp == NULL)
        return -1;
    savePPM(tp, encoded);
    printf("\nSaved to %s\n", targetName);

    return 0;
}

int mainDecrypt(char *fileName)
{
    // Open ppm
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Cannot open ppm file.");
        return -1;
    }
    struct PPM *ppm = getPPM(fp);
    if ((int)ppm == -1)
        return -1;

    // Get key
    printf("\nKey?\n> ");
    char response[100];
    gets(response);
    int key = atoi(response);

    // Decode
    decode(ppm, key);

    return 0;
}

struct PPM * getPPM(FILE *fp)
{
    // Read header code
    if (strcmp(readWord(fp), "P3") != 0)
        return (struct PPM *)-1;

    // Read and parse header values
    int width = atoi(readWord(fp));
    int height = atoi(readWord(fp));
    int maxval = atoi(readWord(fp));

    // Create struct
    struct PPM *ppm = malloc(sizeof(struct PPM) + sizeof(struct Pixel) * width * height);
    ppm->width = width;
    ppm->height = height;
    ppm->maxval = maxval;

    // Read raster
    int i = 0;
    bool done = false;
    while (!done)
    {
        int r = atoi(readWord(fp));
        int g = atoi(readWord(fp));
        char *blue = readWord(fp); // Save last string to check for EOF
        int b = atoi(blue);
        if (blue[0] != 0) // Empty string after EOF
            ppm->raster[i++] = (struct Pixel){r, g, b};
        else
            done = true;
    }

    // return pointer
    return ppm;
}

char * readWord(FILE *fp)
{
    static char word[100];
    // Append letters to array til whitespace reached
    char character;
    int n = 0;
    while ((character = (char)fgetc(fp)) != ' ' && character != '\n' && character != EOF)
    {
        if (character == '#') // Skip comment lines
        {
            // Start fresh on new line
            nextLine(fp);
            return readWord(fp);
        }
        word[n++] = character;
    }
    // End string
    word[n] = '\0';
    return word;
}

void nextLine(FILE *fp)
{
    while (fgetc(fp) != '\n');
}

void showPPM(struct PPM *ptr)
{
    printf("\nPPM Structure\n");
    printf("%s%d\n", "Width: ", ptr->width);
    printf("%s%d\n", "Height: ", ptr->height);
    printf("%s%d\n", "MaxVal: ", ptr->maxval);
    if (SHOW_RASTER)
        printRaster(ptr);
    printf("\n");
}

void printRaster(struct PPM *ptr)
{
    int i = 0;
    for (i = 0; i < ptr->width * ptr->height; i++)
    {
        printf("%d %d %d\n", ptr->raster[i].r, ptr->raster[i].g, ptr->raster[i].b);
    }
}

struct PPM * encode(struct PPM *ppm, char *message, unsigned int mSize, unsigned int secret)
{
    // Create deep copy
    struct PPM *copy = malloc(sizeof(struct PPM) + sizeof(struct Pixel) * ppm->width * ppm->height);
    copy->width = ppm->width;
    copy->height = ppm->height;
    copy->maxval = ppm->maxval;
    int i;
    for(i=0; i<ppm->width*ppm->height; i++)
    {
        copy->raster[i].r = ppm->raster[i].r;
        copy->raster[i].g = ppm->raster[i].g;
        copy->raster[i].b = ppm->raster[i].b;
    }

    // For each character
    int character;
    for (character = 0; character < mSize; character++)
    {
        unsigned char c = message[character];

        // For each bit
        short bit;
        for (bit = 0; bit < 8; bit++)
        {
            unsigned char masked = ((0b00000001 << bit) & c) >> bit;
            setBit(copy, masked, character*8 + bit, secret);
        }
    }

    // Add NULL terminator
    short bit;
    for (bit = 0; bit < 8; bit++)
    {
        setBit(copy, 0, character*8 + bit, secret);
    }

    return copy;
}

int pseudoRand(int seed, int max)
{
    srand(seed);
    return rand() % max;
}

void setBit(struct PPM *ppm, int bit, int position, int secret)
{
    int prand = pseudoRand(position/3 + secret, ppm->width*ppm->height-1);
    switch (position % 3)
    {
    case 0:
        ppm->raster[prand].r = (ppm->raster[prand].r & 0b11111110) | bit;
        break;
    case 1:
        ppm->raster[prand].g = (ppm->raster[prand].g & 0b11111110) | bit;
        break;
    case 2:
        ppm->raster[prand].b = (ppm->raster[prand].b & 0b11111110) | bit;
        break;
    }
}

char * decode(struct PPM *ppm, unsigned int secret)
{
    printf("\n");

    // Array to hold character bits
    unsigned short buffer[8];
    unsigned short bufIndex = 0;

    // For each pixel
    unsigned int pixel;
    for (pixel=0; pixel<ppm->width*ppm->height; pixel++)
    {
        int prand = pseudoRand(pixel + secret, ppm->width*ppm->height-1);
        // For each colour
        unsigned short colour;
        for (colour=0; colour<3; colour++)
        {
            switch (colour)
            {
            case 0:
                buffer[bufIndex++] = ppm->raster[prand].r & 0b00000001;
                break;
            case 1:
                buffer[bufIndex++] = ppm->raster[prand].g & 0b00000001;
                break;
            case 2:
                buffer[bufIndex++] = ppm->raster[prand].b & 0b00000001;
                break;
            }


            // Check for complete character
            if (bufIndex == 8)
            {
                char result = (buffer[7]<<7) + (buffer[6]<<6) + (buffer[5]<<5) + (buffer[4]<<4)
                            + (buffer[3]<<3) + (buffer[2]<<2) + (buffer[1]<<1) + (buffer[0]<<0);
                if (result == 0)
                    goto end;
                printf("%c", result);
                bufIndex = 0;
            }
        }
    }
    end: printf("\n");
}

void savePPM(FILE *fp, struct PPM *ppm)
{
    fputs("P3\n", fp);
    fprintf(fp, "%d\n", ppm->width);
    fprintf(fp, "%d\n", ppm->height);
    fprintf(fp, "%d\n", ppm->maxval);

    int i;
    for (i = 0; i < ppm->width * ppm->height; i++)
    {
        fprintf(fp, "%d ", ppm->raster[i].r);
        fprintf(fp, "%d ", ppm->raster[i].g);
        fprintf(fp, "%d\n", ppm->raster[i].b);
    }
}
