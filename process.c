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
struct Pixel
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

/* An image loaded from a file. */
struct Image
{
    /* TODO: Question 2 */
    int height;
    int width;
    struct Pixel *pixels;
};

/* Free a struct Image */
void free_image(struct Image *img)
{
    /* TODO: Question 3a */
    if (img)
    { /* If we have an image */
        free(img->pixels);
        free(img);
    }
}

/* Opens and reads an image file, returning a pointer to a new struct Image.
 * On error, prints an error message and returns NULL. */
struct Image *load_image(const char *filename)
{
    /* Open the file for reading */
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        fprintf(stderr, "File %s could not be opened.\n", filename);
        return NULL;
    }

    /* Allocate the Image object, and read the image from the file */
    struct Image *img = malloc(sizeof(struct Image)); /* This is allocating memory for the image structure */
    if (!img)
    {
        fclose(f);
        return NULL;
    }

    if (fscanf(f, "HP8 %d %d", &img->height, &img->width) != 2)
    {
        fprintf(stderr, "Invalid format for the image.\n");
        free(img);
        fclose(f);
        return NULL;
    }

    printf("Loaded image: width=%d, height=%d\n", img->width, img->height);  // Debugging log for dimensions

    img->pixels = malloc(img->height * img->width * sizeof(struct Pixel));

    if (!img->pixels)
    {
        free(img);
        fclose(f);
        return NULL;
    }

    if (fread(img->pixels, sizeof(struct Pixel), img->height * img->width, f) != (size_t)(img->height * img->width))
    {
        fprintf(stderr, "Error reading image data.\n");
        free_image(img);
        fclose(f);
        return NULL;
    }

    fclose(f);
    return img;
}

bool save_image(const struct Image *img, const char *filename)
{
    if (img == NULL || img->pixels == NULL)
    {
        fprintf(stderr, "Error: Invalid image data (NULL pointer).\n");
        return false;
    }
    if (img->width <= 0 || img->height <= 0)
    {
        fprintf(stderr, "Error: Invalid image dimensions (width=%d, height=%d).\n", img->width, img->height);
        return false;
    }

    FILE *f = fopen(filename, "wb");
    if (f == NULL)
    {
        perror("File couldn't be opened");
        return false;
    }

    printf("Saving image: width=%d, height=%d\n", img->width, img->height);  // Debugging log for dimensions

    fprintf(f, "HP8 %d %d ", img->height, img->width);  // Ensure height and width are written in the correct order

    size_t total_pixels = (size_t)img->width * img->height;
    if (fwrite(img->pixels, sizeof(struct Pixel), total_pixels, f) != total_pixels)
    {
        perror("Error writing the image data");
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}


/* Write img to file filename. Return true on success, false on error. */

/* Allocate a new struct Image and copy an existing struct Image's contents
 * into it. On error, returns NULL. */
struct Image *copy_image(const struct Image *source)
{
    /* TODO: Question 3d */
    struct Image *copy = malloc(sizeof(struct Image));
    if (!copy)
        return NULL;
    copy->height = source->height;
    copy->width = source->width;
    copy->pixels = malloc(copy->height * copy->width * sizeof(struct Pixel));
    if (!copy->pixels)
    {
        free(copy);
        return NULL;
    }
    memcpy(copy->pixels, source->pixels, copy->height * copy->width * sizeof(struct Pixel));
    return copy;
}

static int color_to_sort = 2;

int compare_pixels(const void *x, const void *y)
{
    struct Pixel *p1 = (struct Pixel *)x;
    struct Pixel *p2 = (struct Pixel *)y;

    switch (color_to_sort)
    {
    case 0:
        return p1->red, p2->red;
    case 1:
        return p1->green ,p2->green;
    case 2:
        return p1->blue , p2->blue;
    default:
        return 0;
    }
}

/* Perform your first task.
 * (TODO: Write a better comment here, and rename the function.
 * You may need to add or change arguments depending on the task.)
 * Returns a new struct Image containing the result, or NULL on error. */
#include <stdlib.h>
#include <string.h>

// Function to compare two integers for qsort
int compare_ints(const void *a, const void *b) {
    return (*(unsigned char *)a - *(unsigned char *)b);
}

struct Image *apply_MEDIAN(const struct Image *source) {
    if (!source || !source->pixels) return NULL;

    // Step 1: Copy the image
    struct Image *filtered = copy_image(source);
    if (!filtered) return NULL;

    // Step 2: Iterate through each pixel, avoiding edges
    for (int y = 1; y < source->height - 1; y++) {
        for (int x = 1; x < source->width - 1; x++) {
            unsigned char red_vals[5], green_vals[5], blue_vals[5];

            // Step 3: Collect 5-pixel neighborhood
            struct Pixel neighbors[5] = {
                source->pixels[y * source->width + x],       // Center pixel
                source->pixels[(y - 1) * source->width + x], // Above
                source->pixels[(y + 1) * source->width + x], // Below
                source->pixels[y * source->width + (x - 1)], // Left
                source->pixels[y * source->width + (x + 1)]  // Right
            };

            // Step 4: Extract color channels separately
            for (int i = 0; i < 5; i++) {
                red_vals[i] = neighbors[i].red;
                green_vals[i] = neighbors[i].green;
                blue_vals[i] = neighbors[i].blue;
            }

            // Step 5: Sort each color channel
            qsort(red_vals, 5, sizeof(unsigned char), compare_ints);
            qsort(green_vals, 5, sizeof(unsigned char), compare_ints);
            qsort(blue_vals, 5, sizeof(unsigned char), compare_ints);

            // Step 6: Assign median values to the copied image
            struct Pixel *target_pixel = &filtered->pixels[y * source->width + x];
            target_pixel->red = red_vals[2];   // Median of red
            target_pixel->green = green_vals[2]; // Median of green
            target_pixel->blue = blue_vals[2];  // Median of blue
        }
    }

    return filtered;
}



/* Perform your second task.
 * (TODO: Write a better comment here, and rename the function.
 * You may need to add or change arguments depending on the task.)
 * Returns true on success, or false on error. */
bool apply_HIST(const struct Image *source)
{
    /* TODO: Question 5 */
    int histogram[256] = {0};
    for (int i = 0; i < source->height * source->width; i++)
    {
        histogram[source->pixels[i].red]++;
        histogram[source->pixels[i].green]++;
        histogram[source->pixels[i].blue]++;
    }

    for (int j = 0; j < 256; j++)
    {
        printf("Value %d: %d pixels\n ", j, histogram[j]);
    }
    return true;
}

int main(int argc, char *argv[])
{
    /* Initialise the random number generator, using the time as the seed */
    /* TODO: Remove this if you don't need to use rand() */
    srand(time(NULL));

    /* Check command-line arguments */
    if (argc != 3)
    {
        fprintf(stderr, "Usage: process INPUTFILE OUTPUTFILE\n");
        return 1;
    }

    /* Load the input image */
    struct Image *in_img = load_image(argv[1]);
    if (in_img == NULL)
    {
        return 1;
    }

    /* Apply the first process */
    struct Image *out_img = apply_MEDIAN(in_img);
    if (out_img == NULL)
    {
        fprintf(stderr, "First process failed.\n");
        free_image(in_img);
        return 1;
    }

    /* Apply the second process */
    if (!apply_HIST(out_img))
    {
        fprintf(stderr, "Second process failed.\n");
        free_image(in_img);
        free_image(out_img);
        return 1;
    }

    /* Save the output image */
    if (!save_image(out_img, argv[2]))
    {
        fprintf(stderr, "Saving image to %s failed.\n", argv[2]);
        free_image(in_img);
        free_image(out_img);
        return 1;
    }

    free_image(in_img);
    free_image(out_img);
    return 0;
}
