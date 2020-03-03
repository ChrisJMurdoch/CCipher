
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/main.h"

int main(int argc, char *argv[])
{
    // Open file
    FILE *fp = fopen("./resources/home-cat.ppm", "r");
    if(fp == NULL)
        return -1;

    // Read header code
    if ( strcmp( readWord(fp), "P3" ) != 0)
        return -1;
    
    // Skip ppm comment - temporary fix
    nextLine(fp);

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

    // Display
    showPPM(ppm);

    printf("Program exiting successfully.\n");
    return 0;
}

char * readWord(FILE *fp)
{
    static char word[100];
    // Append letters to array til whitespace reached
    char character;
    int n = 0;
    while ( (character = (char)fgetc(fp)) != ' ' && character != '\n' && character != EOF )
        word[n++] = character;
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
    //printRaster(ptr);
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

