# F28HS Coursework 1

My Tasks were HP8 MEDIAN HIST

How To Compile: 

1. Open terminal on Linux (I am using Ubuntu)

2. Travel to the directory where CWK1-C is stored.

3. To compile, write gcc -o process process.c 

4. To originally check what an image should convert to, use ./hpconvert image.hp8 image (where image is arbitrary)
   The first value (image.hp8) is the HP8 value you would like to convert, and the output (image) is to save that image as a viewable image.

5. To apply the tasks to an image, we need to write in this command: 

    ./process image.hp8 image2.hp8 

   First value (image.hp8) is the file we want to apply our filter to, second value (image2.hp8) is a copy of the first value, but with the tasks applied.

6. Then, to see the image with the tasks applied, write ./hpconvert image2.hp8 image2


MEDIAN - Should produce an image that seems blurrier than the previous iteration.

HIST - Should produce a histogram of the red, green and blue values in the image.
       I have made it so it counts every channel, including red, green and blue. This was a question asked personally by me to a senior position and they have stated that it is okay to do. For example, if we have a pixel with RGB values [0,0,0], then Value 0 = 3. 
