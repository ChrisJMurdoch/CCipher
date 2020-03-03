
#pragma once

struct Pixel
{
    int r;
    int g;
    int b;
};
struct PPM {
    int height;
    int width;
    int maxval;
    struct Pixel raster[];
};

char * readWord(FILE *fp);
void nextLine(FILE *fp);
void showPPM(struct PPM *im);
void printRaster(struct PPM *im);

struct PPM * getPPM(FILE *f);
struct PPM * encode(struct PPM *im, char *message, unsigned int mSize, unsigned int secret);
char * decode(struct PPM *im, unsigned int secret);