/* This coursework specification, and the example code provided during the
 * course, is Copyright 2025 Heriot-Watt University.
 * Distributing this coursework specification or your solution to it outside
 * the university is academic misconduct and a violation of copyright law. */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* The RGB values of a pixel. Making it an unsigned char means the values are from 0-255, which is what my HP8 format follows. */
struct Pixel
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

/* An image loaded from a file. */
struct Image
{
    int height ,  width;  // Defines the height and width of the image 
    struct Pixel *pixels; // Pointer to an array of "Pixel"
    char *file_names;
};

/* Free a struct Image */
void free_image(struct Image *img)
{
    if (img) /* If we have an image */
    { 
        free(img->pixels); // Free the allocated memory for the pixels.
        free(img->file_names);
        free(img);         // Free the memory for Image
    }
}

/* Opens and reads an image file, returning a pointer to a new struct Image.
 * On error, prints an error message and returns NULL. */
struct Image *load_image(const char *filename)
{
    /* Open the file for reading */
    FILE *f = fopen(filename, "rb");
    if (f == NULL) // If there is no file to open
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

    if (fscanf(f, "HP8 %d %d\n", &img->height, &img->width) != 2) //If the image dimensions are not in the format HP8 height width, then return an error and free the image.
    {
        fprintf(stderr, "Invalid format for the image.\n");
        free(img);
        fclose(f);
        return NULL;
    }

    printf("Loaded image: width=%d, height=%d\n", img->width, img->height);  // This is a debugging comment to check that the code is loading the image dimenstions correctly

    img->pixels = malloc(img->height * img->width * sizeof(struct Pixel)); // This is to allocate memory for the pixel data (memory for the number of pixels in the image.)

    if (!img->pixels) // If our memory allocation has failed
    {
        free(img);
        fclose(f);
        return NULL; // Return NULL to indicate a failure
    }

    if (fread(img->pixels, sizeof(struct Pixel), img->height * img->width, f) != (size_t)(img->height * img->width)) // If pixel data is not read correctly (if the 2 sizes are not the same)
    {
        fprintf(stderr, "Error reading image data.\n");
        free_image(img);
        fclose(f);
        return NULL;
    }

    fclose(f); // Close the file after reading
    return img; // This returns the loaded image
}

/* Saves the loaded image to a file. Is a boolean so returns true if saving an image is a success. */
bool save_image(const struct Image *img, const char *filename)
{
    if (img == NULL || img->pixels == NULL) // If either the image or the pixels are null, then print an error message.
    {
        fprintf(stderr, "Error: Invalid image data (NULL pointer).\n");
        return false;
    }
    if (img->width <= 0 || img->height <= 0) // Width and height have to be bigger than 0 (cannot have a negative length), so if statement to return an error if image dimensions are invalid.
    {
        fprintf(stderr, "Error: Invalid image dimensions (width=%d, height=%d).\n", img->width, img->height);
        return false;
    }

    FILE *f = fopen(filename, "wb"); // Opens the file in write-binary
    if (f == NULL) // If the file opening has failed, return an error/
    {
        perror("File couldn't be opened");
        return false;
    }

    printf("Saving image: width=%d, height=%d\n", img->width, img->height);  // Debugging log to check dimensions are being saved correctly.

    fprintf(f, "HP8 %d %d ", img->height, img->width);  // Ensure height and width are written in the correct order - height first, width second.

    size_t total_pixels = (size_t)img->width * img->height; // Calculates the total number of pixels in the image.
    if (fwrite(img->pixels, sizeof(struct Pixel), total_pixels, f) != total_pixels) // If the writing fails, meaning that that the total number of pixels are different, then we return an error.
    {
        perror("Error writing the image data");
        fclose(f);
        return false;
    }

    fclose(f); // Close the file after writing.
    return true; // Means that the function works
}


/* Write img to file filename. Return true on success, false on error. */

/* Allocate a new struct Image and copy an existing struct Image's contents
 * into it. On error, returns NULL. */
struct Image *copy_image(const struct Image *source)
{
    struct Image *copy = malloc(sizeof(struct Image)); // Allocate memory for a new image structure.
    if (!copy) // If the memory allocation fails, then we return null
        return NULL;
    
    copy->height = source->height; // Copy the height of the source image
    copy->width = source->width;   // Copy the width of the source image
    
    copy->pixels = malloc(copy->height * copy->width * sizeof(struct Pixel)); // Allocates more memory for the pixels
    if (!copy->pixels) // If the memory allocation fails
    {
        free(copy);
        return NULL;
    }
    memcpy(copy->pixels, source->pixels, copy->height * copy->width * sizeof(struct Pixel)); // Copy ALL the pixel date from the source image
    return copy; // Return the source image.
}


// /* 
//    My Personalised Task 1 is to create a median filter. What this filter does it it will blur the image by replacing each pixel with the meidan of the surrounding 5 pixels
//    (cross-head). We create a copy of the image to avoid modifying the original image whilst processing. Then we should iterate through each pixel minus the border pixels (because there isn't enough neighbours.)
//    Then we collect the current pixel and its 4 surrounding neighbours using the cross head. What I then do is sort the red, green and blue channels separately, and to get the median i use qsort - a builtin.
//    After, we replace the current pixel with the median value for each color channelm and then return the filtered image. It does remove noise.  */

// // Function to compare two integers for qsort - Makes sure that the value is always positive
int compare_ints(const void *a, const void *b) {
    return (*(unsigned char *)a - *(unsigned char *)b);
}

// Median filter (Personalised Task 1)
struct Image *apply_MEDIAN(const struct Image *source) {
    if (!source || !source->pixels) return NULL; // ONLY to check if we don't have a source image 

    struct Image *filtered = copy_image(source); // Create a new image that just copies the original image 
    if (!filtered) return NULL; // If creation fails return null

    // O Complexity is N -> because we iterating through each pixel only once, even if we have 2 for loops.
    for (int y = 1; y < source->height - 1; y++) {  // Loops through each row, excluding edges
        for (int x = 1; x < source->width - 1; x++) { // Loops through each column, excluding edges.
            unsigned char red_vals[5], green_vals[5], blue_vals[5]; // Assigns arrays to find medians of each colour channel

            // Collect the 5 pixels via cross-head method
            struct Pixel neighbors[5] = {
                source->pixels[y * source->width + x],       // Center pixel
                source->pixels[(y - 1) * source->width + x], // Above pixel
                source->pixels[(y + 1) * source->width + x], // Below pixel
                source->pixels[y * source->width + (x - 1)], // Left pixel
                source->pixels[y * source->width + (x + 1)]  // Right pixel
            };

            // Extract every value from it's neighbours per colour channel
            for (int i = 0; i < 5; i++) {
                red_vals[i] = neighbors[i].red;
                green_vals[i] = neighbors[i].green;
                blue_vals[i] = neighbors[i].blue;
            }

            //  Quicksort each color channel
            qsort(red_vals, 5, sizeof(unsigned char), compare_ints);
            qsort(green_vals, 5, sizeof(unsigned char), compare_ints);
            qsort(blue_vals, 5, sizeof(unsigned char), compare_ints);

            //  Assign each median value to the copied image
            struct Pixel *target_pixel = &filtered->pixels[y * source->width + x]; // Assigns median pixel to the middle pixel
            target_pixel->red = red_vals[2];   // Median of red
            target_pixel->green = green_vals[2]; // Median of green
            target_pixel->blue = blue_vals[2];  // Median of blue
        }
    }

    return filtered; // Return the filtered image.
}



/* This is my second personalised task, or the Histogram. My iteration differs a little bit from the prompt given in  the coursework, however I had confirmation that as long as 
   the code is consistent and I get consistent results each time, that is okay.
   
   How my histogram works is I have an array for each colour channel, which for every pixel, counts how many values in RGB that contains the value. For example, if I had [0,0,0] then Value 0 would have 1 Red, 1 Green, 1 Blue.
   This differs from the original as I have decided against a greyscale conversion using */
bool apply_HIST(const struct Image *source)
{

    int hist_red[256] = {0}; // Histogram for the red values
    int hist_green[256] = {0}; // Histogram for the green values
    int hist_blue[256] = {0}; // Histogram for the blue values

    // This for loop counts the number of occurences of each pixel RGB value
    for (int i = 0; i < source->height * source->width; i++)
    { 
        hist_red[source->pixels[i].red]++;  // Increment red histogram
        hist_green[source->pixels[i].green]++; // Increment green histogram
        hist_blue[source->pixels[i].blue]++;  // Increment blue histogram
    }

    for (int j = 0; j < 256; j++) // Loop to print all the values
    {
        printf("Value %3d - Red: %d Green: %d Blue: %d\n", j, hist_red[j], hist_green[j], hist_blue[j]);
    }
    return true; // Return true if it's a success.
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc % 2 == 0) {
        fprintf(stderr, "Usage: process IN1 OUT1 IN2 OUT2 ...\n");
        return 1;
    }

    int num_images = (argc - 1) / 2; // Tells the program the number of images to process (for the loops)
    struct Image **images = malloc(num_images * sizeof(struct Image *)); // images now have been allocated an array for pointers, this means we can have multiple images being processed. This is using a dynamically allocated array.
    
    if (!images) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    for (int i = 0; i < num_images; i++) {
        images[i] = load_image(argv[2 * i + 1]); // Load each image up (input AND output)
        if (!images[i]) {
            fprintf(stderr, "Failed to load image %s\n", argv[2 * i + 1]); 
            return 1;
        }
        images[i]->file_names = strdup(argv[2 * i + 2]); // Store the filename of the output image.
    }

    for (int i = 0; i < num_images; i++) { // Loop to traverse through each image and apply the filters to them.
        struct Image *out_img = apply_MEDIAN(images[i]);  // Apply the median filter
        if (!out_img) {
            fprintf(stderr, "Process failed for image %s.\n", argv[2 * i + 1]);
            return 1;
        }

        if (!apply_HIST(out_img) || !save_image(out_img, images[i]->file_names)) {
            fprintf(stderr, "Error processing %s.\n", argv[2 * i + 1]);
            return 1;
        }

        free_image(out_img); // Free the processed image
    }

    for (int i = 0; i < num_images; i++) {
        free_image(images[i]); // Free the original images
    }
    free(images);
    
    return 0;
}
