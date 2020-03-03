
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/main.h"

const bool SHOW_RASTER = false;

int main(int argc, char *argv[])
{
    // Open ppm
    FILE *fp = fopen("./resources/home-cat.ppm", "r");
    if(fp == NULL)
        return -1;
    struct PPM * ppm = getPPM(fp);
    if ((int)ppm == -1)
        return -1;

    // Display
    showPPM(ppm);

    // Write ppm
    FILE *tp = fopen("./target/home-cat.ppm", "w");
    if(tp == NULL)
        return -1;
    if (savePPM(tp, ppm) == -1)
        return -1;

    // Exit
    printf("Program exiting successfully.\n");
    return 0;
}

struct PPM * getPPM(FILE * fp)
{
    // Read header code
    if ( strcmp( readWord(fp), "P3" ) != 0)
        return (struct PPM *) -1;

    // Read and parse header values
    int width = atoi( readWord(fp) );
    int height = atoi( readWord(fp) );
    int maxval = atoi( readWord(fp) );

    // Create struct
    struct PPM *ppm = malloc( sizeof(struct PPM) + sizeof(struct Pixel) * width*height );
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
        char *blue = readWord(fp);// Save last string to check for EOF
        int b = atoi(blue);
        if (blue[0] != 0) // Empty string after EOF
            ppm->raster[i++] = (struct Pixel) { r, g, b };
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
    while ( (character = (char)fgetc(fp)) != ' ' && character != '\n' && character != EOF )
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
    int i =0;
    for ( i=0; i<ptr->width*ptr->height; i++ )
    {
        printf( "R:%d, G:%d, B:%d\n", ptr->raster[i].r, ptr->raster[i].g, ptr->raster[i].b );
    }
}

int savePPM(FILE *fp, struct PPM *ppm)
{
    fputs("P3\n", fp);
    fprintf(fp, "%d\n", ppm->width);
    fprintf(fp, "%d\n", ppm->height);
    fprintf(fp, "%d\n", ppm->maxval);

    int i;
    for ( i=0; i<ppm->width*ppm->height; i++)
    {
        fprintf(fp, "%d ", ppm->raster[i].r);
        fprintf(fp, "%d ", ppm->raster[i].g);
        fprintf(fp, "%d\n", ppm->raster[i].b);
    }

    return 0;
}
