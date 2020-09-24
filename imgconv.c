#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "stb_image.h"
#include "stb_image_write.h"

int readmatrix(size_t rows, size_t cols, double (*a)[cols], const char* filename)
{

    FILE *pf;
    pf = fopen (filename, "r");
    if (pf == NULL)
        return 0;

    for(size_t i = 0; i < rows; ++i)
    {
        for(size_t j = 0; j < cols; ++j)
            fscanf(pf, "%lf", a[i] + j);
    }


    fclose (pf);
    return 1;
}

int main(){



  char *argv [] =  {"lena1.jpeg", "filter_gauss.txt", "lena1_gauss_avg1.jpeg", "3"};
  //char *argv [] =  {"edificio1.jpeg", "filter_gauss.txt", "edificio_gauss_lum.jpeg", "3"};
  //char *argv [] =  {"paisaje1.jpeg", "filter_gauss.txt", "paisaje_gauss_lum.jpeg", "3"};

	clock_t tic_t = clock(); //timer for total process

	char *imgpath = argv[0];
	char *filterpath = argv[1];
	char *imgrespath = argv[2];
	char *filtsize = argv[3];

	int w, h, bpp, i, j, m, n, cc = 3;
	unsigned char sum;

	double a[atoi(filtsize)][atoi(filtsize)];
	readmatrix(atoi(filtsize), atoi(filtsize), a, filterpath); //creation of the filter matrix

	uint8_t* image = stbi_load(imgpath, &w, &h, &bpp, cc); //creation of the image vector
	size_t img_size = w * h * cc;

	int gray_channels = 1;
	size_t gray_img_size = w * h * gray_channels;

	unsigned char *gray_img = malloc(gray_img_size*10);
	unsigned char *new_img_gray = malloc(gray_img_size*10);

    //transforming of RGB image to gray scale image
    clock_t tic_g = clock();
	for(unsigned char *p = image, *pg = gray_img; p != image + img_size; p += cc, pg += gray_channels) {
		//*pg = (uint8_t)((*p + *(p + 1) + *(p + 2))/3.0); //average
		*pg = (uint8_t)((*p*0.21 + *(p + 1)*0.72 + *(p + 2)*0.07)); //luminosity
	}
	clock_t toc_g = clock();

	//convolution of the gray image and the filter
	clock_t tic_c = clock();
	int temp = atoi(filtsize)-1;

	for (unsigned char *p = gray_img, *pg = new_img_gray; p != gray_img + gray_img_size; p++, pg++){
		sum = 0;
	//	for (i = temp; i >= 0; i--){ //convolution for digital images
	//		for (j = temp; j >= 0; j--){
	//			sum += a[i][j] * *(p + 1 + (atoi(filtsize)-(j+1)) + (w*(atoi(filtsize)-(i+1))));
	//		}
	//	}
		for (i = 0; i <= temp; i++){  //convolution spatial transform
			for (j = 0; j <= temp; j++){
				sum += a[i][j] * *(p + 1 + (atoi(filtsize)-(j+1)) + (w*(atoi(filtsize)-(i+1))));
			}
		}

		//save convolution result into the new image
		*pg = (uint8_t)sum;
	}
	clock_t toc_c = clock(); //convolution time

	stbi_write_jpg(imgrespath, w, h, gray_channels, new_img_gray, 100);
	stbi_image_free(image);
	free(gray_img);
	free(new_img_gray);
	clock_t toc_t = clock(); //total time

	//times
    printf("Total gray transformation time: %f seconds\n", (double)(toc_g - tic_g) / CLOCKS_PER_SEC);
	printf("Total convolution time: %f seconds\n", (double)(toc_c - tic_c) / CLOCKS_PER_SEC);
	printf("Total time elapsed: %f seconds\n", (double)(toc_t - tic_t) / CLOCKS_PER_SEC);

	return 0;
}
