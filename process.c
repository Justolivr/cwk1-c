/* This coursework specification, and the example code provided during the
 * course, is Copyright 2025 Heriot-Watt University.
 * Distributing this coursework specification or your solution to it outside
 * the university is academic misconduct and a violation of copyright law. */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* The RGB values of a pixel. */
struct Pixel {
   unsigned char red;
   unsigned char green;
   unsigned char blue;
};

/* An image loaded from a file. */
struct Image {
    /* TODO: Question 2 */
    int height;
    int width;
    struct Pixel * pixels;
};

/* Free a struct Image */
void free_image(struct Image *img)
{
    /* TODO: Question 3a */
    if(img){ /* If we have an image */
        free(img -> pixels);
        free(img);
    }
}

/* Opens and reads an image file, returning a pointer to a new struct Image.
 * On error, prints an error message and returns NULL. */
struct Image *load_image(const char *filename)
{
    /* Open the file for reading */
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "File %s could not be opened.\n", filename);
        return NULL;
    }

    /* Allocate the Image object, and read the image from the file */
    /* TODO: Question 3b */
    struct Image *img = malloc(sizeof(struct Image)); /* This is allocating some memory to the size of the image*/
    if (!img) {
        fclose(f);
        return NULL;
    }

    if(fscan(f, "%d , %d", &img->width, &img->height) != 2){
        fprint(stderr, "Invalid format for the image.\n ");
        free(img);
        fclose(f);
        return NULL;
    }

    img->pixels = malloc(img->height*img->width* sizeof(struct Pixel));
    
    if (!img->pixels) {
        free(img);
        fclose(f);
        return NULL;
    }

    if (fread(img->pixels, sizeof(struct Pixel), img->height * img->width, f) != (size_t)(img->height * img->width)) {
        fprintf(stderr, "Error reading image data.\n");
        free_image(img);
        fclose(f);
        return NULL;
    }
    /* Close the file */
     fclose(f);

   
    if (img == NULL) {
        fprintf(stderr, "File %s could not be read.\n", filename);
        return NULL;
    }

    return img;
}

/* Write img to file filename. Return true on success, false on error. */
bool save_image(const struct Image *img, const char *filename)
{
    /* TODO: Question 3c */
    FILE *f =fopen(filename, "wb"); 
    fprintf(f, " %d %d", img->height, img->width);
    if(fwrite(img->pixels, sizeof(struct pixels), img->height*img->width, f) != (size_t)(img->height, img->width)){
        return false;
    }
    return true;
}

/* Allocate a new struct Image and copy an existing struct Image's contents
 * into it. On error, returns NULL. */
struct Image *copy_image(const struct Image *source)
{
    /* TODO: Question 3d */
     struct Image *copy = malloc(sizeof(struct Image));
    if (!copy) return NULL;
    copy->height = source->height;
    copy->width = source->width;
    copy->pixels = malloc(copy->height * copy->width * sizeof(struct Pixel));
    if (!copy->pixels) {
        free(copy);
        return NULL;
    }
    memcpy(copy->pixels, source->pixels, copy->height * copy->width * sizeof(struct Pixel));
    return copy;
}

/* Perform your first task.
 * (TODO: Write a better comment here, and rename the function.
 * You may need to add or change arguments depending on the task.)
 * Returns a new struct Image containing the result, or NULL on error. */
struct Image *apply_MEDIAN(const struct Image *source)
{
    /* TODO: Question 4 */
    return NULL;
}

/* Perform your second task.
 * (TODO: Write a better comment here, and rename the function.
 * You may need to add or change arguments depending on the task.)
 * Returns true on success, or false on error. */
bool apply_HIST(const struct Image *source)
{
    /* TODO: Question 5 */
    return false;
}

int main(int argc, char *argv[])
{
    /* Initialise the random number generator, using the time as the seed */
    /* TODO: Remove this if you don't need to use rand() */
    srand(time(NULL));

    /* Check command-line arguments */
    if (argc != 3) {
        fprintf(stderr, "Usage: process INPUTFILE OUTPUTFILE\n");
        return 1;
    }

    /* Load the input image */
    struct Image *in_img = load_image(argv[1]);
    if (in_img == NULL) {
        return 1;
    }

    /* Apply the first process */
    struct Image *out_img = apply_BLUR(in_img);
    if (out_img == NULL) {
        fprintf(stderr, "First process failed.\n");
        free_image(in_img);
        return 1;
    }

    /* Apply the second process */
    if (!apply_NORM(out_img)) {
        fprintf(stderr, "Second process failed.\n");
        free_image(in_img);
        free_image(out_img);
        return 1;
    }

    /* Save the output image */
    if (!save_image(out_img, argv[2])) {
        fprintf(stderr, "Saving image to %s failed.\n", argv[2]);
        free_image(in_img);
        free_image(out_img);
        return 1;
    }

    free_image(in_img);
    free_image(out_img);
    return 0;
}
