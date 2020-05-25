
bmp2array4bit.py creates C (or C++) code that contains two arrays for adding images to four-bit sprites.  See Sprite_image_4bit for an example.

It is loosely based on Spark Fun's bmp2array script.

You'll need python 3.6 (the original use Python 2.7)

usage: python bmp2array4bit.py [-v] star.bmp [-o myfile.c]

Create the bmp file in Gimp from any image by:

. Remove the alpha channel (if it has one)
        Layer -> Transparency -> Remove Alpha Channel
. Set the mode to indexed.
        Image -> Mode -> Indexed...
. Select Generate optimum palette with 16 colors (max)
. Export the file with a .bmp extension. Do NOT select options:
    . Run-Length Encoded
    . Compatibility Options: "Do not write color space information" 
    . There are no Advanced Options available with these settings

I don't have photoshop so cannot help you with that.

The first array produced is the palette for the image.
The second is the image itself.