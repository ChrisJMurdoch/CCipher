
#pragma once

// RGP Pixel structure
struct Pixel
{
    int r;
    int g;
    int b;
};

// PPM format structure
struct PPM {
    int height;
    int width;
    int maxval;
    struct Pixel raster[];
};

// Mains
int mainEncrypt();
int mainDecrypt();

// Get ppm
struct PPM * getPPM(FILE *fp);
char * readWord(FILE *fp);
void nextLine(FILE *fp);

// Show ppm
void showPPM(struct PPM *im);
void printRaster(struct PPM *im);

// Save ppm
void savePPM(FILE *fp, struct PPM *ppm);

// Encode
struct PPM * encode(struct PPM *im, char *message, unsigned int mSize, unsigned int secret);
void setBit(struct PPM *ppm, int bit, int position, int secret);

// Decode
char * decode(struct PPM *im, unsigned int secret);
