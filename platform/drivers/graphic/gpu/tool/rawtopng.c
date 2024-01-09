#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <png.h>
#include <stdlib.h>
// This function actually writes out the PNG image file. The string 'title' is
// also written into the image file

//raw file format:
//header:
        //width  = read_long(fp);
        //height = read_long(fp);
        //stride = read_long(fp);
        //format = read_long(fp);


// Read a 32-bit signed integer.
static int read_long(FILE *fp)
{
    unsigned char b0, b1, b2, b3; /* Bytes from file */

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}



/*
fun:    根据宽,高,标题将每个像素的RGB值在指定路径生成png文件
return: int型,0表示正确,1表示出错
arg[0]:    filename,生成的文件名字
arg[1]: width,图片宽
arg[2]: height,图片高
arg[3]: title,标题
*/
int writeImage(char* filename, int width, int height, char* fbbuf ,char* title)
{
     int code = 0;
     FILE *fp = NULL;
     png_structp png_ptr = NULL;
     png_infop info_ptr = NULL;
     png_bytep row = NULL;
     char *fbr = 0;
     // Open file for writing (binary mode)
     fp = fopen(filename, "wb");
     if (fp == NULL) {
         fprintf(stderr, "Could not open file %s for writing\n", filename);
         code = 1;
         goto finalise;
     }

     // Initialize write structure
     png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
     if (png_ptr == NULL) {
         fprintf(stderr, "Could not allocate write struct\n");
         code = 1;
         goto finalise;
     }

     // Initialize info structure
     info_ptr = png_create_info_struct(png_ptr);
     if (info_ptr == NULL) {
         fprintf(stderr, "Could not allocate info struct\n");
         code = 1;
          goto finalise;
     }
     // Setup Exception handling
     if (setjmp(png_jmpbuf(png_ptr))) {
         fprintf(stderr, "Error during png creation\n");
         code = 1;
         goto finalise;
     }

     png_init_io(png_ptr, fp);

     // Write header (8 bit colour depth)
     png_set_IHDR(png_ptr, info_ptr, width, height,
         8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

     // Set title
     if (title != NULL) {
         png_text title_text;
         title_text.compression = PNG_TEXT_COMPRESSION_NONE;
         title_text.key = "Title";
         title_text.text = title;
         png_set_text(png_ptr, info_ptr, &title_text, 1);
     }
     png_write_info(png_ptr, info_ptr);

     // Allocate memory for one row (4 bytes per pixel - RGBA)
     row = (png_bytep)malloc(4 * width * sizeof(png_byte));

     // Write image data
     int x, y;

     for (y = 0; y<height; y++) {
         fbr = fbbuf + width*4*y;
        for (x = 0; x<width; x++) {
            row[x * 4 + 0] = fbr[x * 4 + 0];
            row[x * 4 + 1] = fbr[x * 4 + 1];
            row[x * 4 + 2] = fbr[x * 4 + 2];
            row[x * 4 + 3] = fbr[x * 4 + 3];
         }
         png_write_row(png_ptr, row);
     }

     // End write
     png_write_end(png_ptr, NULL);

 finalise:
     if (fp != NULL) fclose(fp);
     if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
     if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
     if (row != NULL) free(row);

     return code;
 }

int main(int argc, char *argv[])
{
  // Make sure that the output filename argument has been provided
   char* fbbuf = NULL;

   int fbbufsize = 0;

   int32_t raw_width;                  /*! Width of the buffer in pixels. */
   int32_t raw_height;                 /*! Height of the buffer in pixels. */
   int32_t raw_stride;
   int32_t format;

   FILE *fp = NULL;
   if (argc != 3) {
         fprintf(stderr, "usage: \r\n   raw2png pngfilename rawfile  \n");
         return 1;
     }

     // Specify an output image size

     fp = fopen(argv[2],"rb");

     raw_width  = read_long(fp);
     raw_height = read_long(fp);
     raw_stride = read_long(fp);
     format = read_long(fp);
     printf("raw_width:%d", raw_width);
     printf("raw_height:%d", raw_height);
     printf("raw_stride:%d", raw_stride);
     printf("format:%d", format);
     fbbufsize = raw_stride*raw_height;


     fbbuf = (char*)malloc(fbbufsize);
     fread(fbbuf,fbbufsize, 1, fp );
     fclose(fp);
#if 0 //test
     char * fbr;
     int x, y;
     unsigned char red = 0xff;
     unsigned char green = 0xff;
     unsigned char blue  = 0xff;
     for (y = 0; y<height; y++) {
         fbr = fbbuf + width*4*y;
        for (x = 0; x<width; x++) {
            if (y%4 == 0){

            fbr[x * 4 + 0] = red;
            fbr[x * 4 + 1] = 0x00;
            fbr[x * 4 + 2] = 0x00;
            fbr[x * 4 + 3] = 0xff;
            }else{
            fbr[x * 4 + 0] = 0x00;
            fbr[x * 4 + 1] = green;
            fbr[x * 4 + 2] = 0x00;
            fbr[x * 4 + 3] = 0xff;
            }
         }
     }
#endif



     // Save the image to a PNG file
     // The 'title' string is stored as part of the PNG file
     printf("Saving PNG\n");
     int result = writeImage(argv[1], raw_width, raw_height, fbbuf,"This is my test image");
     if (result)
     {
          printf("Saving err\n");
      }

      //// Free up the memorty used to store the image
      //free(buffer);

      return result;
  }
