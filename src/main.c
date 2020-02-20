
#include <stdio.h>
#include <stdbool.h>

#include "../include/main.h"

int main(int argc, char *argv[])
{
    printf("\nOpening file..\n");
    FILE *fp = fopen("./resources/home-cat.ppm", "r");

    printf("\nReading header..\n");
    printf( "%s%s%s", "PPM code: ", readWord(fp), "\n" );
    nextLine(fp); // Skip ppm comment - temporary fix
    printf( "%s%s%s", "Width: ", readWord(fp), "\n" );
    printf( "%s%s%s", "Height: ", readWord(fp), "\n" );
    printf( "%s%s%s", "MaxVal: ", readWord(fp), "\n" );

    printf("\nReading raster..\n");
    int count = 0;
    bool done = false;
    while (!done)
    {
        char *word = readWord(fp);
        if (word[0] != '\0')
            count++;
        else
            done = true;
    }
    printf("Raster size: %d.", count);
    printf("\n\nDone.\n");
}

char * readWord(FILE *fp)
{
    static char word[100];
    int n = 0;
    char character;
    // Append letters to array til whitespace reached
    while ( (character = (char)fgetc(fp)) != ' ' && character != '\n' && character != EOF )
        word[n++] = character;
    // End string
    word[n] = '\0';
    return word;
}

void nextLine(FILE *fp)
{
    while (fgetc(fp) != '\n')
        printf(" "); // This dummy expresssion seems to cut the compile and pre-run time by ~90% - No idea why
    printf("\n");
}
