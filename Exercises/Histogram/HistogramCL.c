#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "FreeImage.h"

#include <CL/cl.h>

#include "readKernel.h"

#define BINS 256


void HistogramCPU(unsigned char *imageIn, unsigned int *histogram, int width, int height)
{

	memset(histogram, 0, BINS * sizeof(unsigned int));

	/*
	for (int i = 0; i < (height); i++)
		for (int j = 0; j < (width); j++)
		{
			histogram[imageIn[i*width + j]]++;
		}
	*/
}

void printHistogram(unsigned int *histogram) {
	printf("Color\tNum. of occ.\n");
	for (int i = 0; i < BINS; i++) {
		printf("%d\t%d\n", i, histogram[i]);
	}



}


int main(void)
{

	FIBITMAP *imageBitmap = FreeImage_Load(FIF_PNG, "test.png", 0);
	FIBITMAP *imageBitmapGrey = FreeImage_ConvertToGreyscale(imageBitmap);
	int width = FreeImage_GetWidth(imageBitmapGrey);
	int height = FreeImage_GetHeight(imageBitmapGrey);
	int pitch = FreeImage_GetPitch(imageBitmapGrey);

	unsigned char *imageIn = (unsigned char *)malloc(height*width * sizeof(unsigned char));
	unsigned int *histogram = (unsigned int *)malloc(BINS * sizeof(unsigned int));

	FreeImage_ConvertToRawBits(imageIn, imageBitmapGrey, pitch, 8, 0xFF, 0xFF, 0xFF, TRUE);

	FreeImage_Unload(imageBitmapGrey);
	FreeImage_Unload(imageBitmap);


	HistogramCPU(imageIn, histogram, width, height);
	printHistogram(histogram);

	return 0;
}

