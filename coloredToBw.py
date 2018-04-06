#! /usr/bin/python

from PIL import Image

# Opening input image
img = Image.open('image.png')

# Convert to greyscale image
img = img.convert('LA')

# To be saved in PGM format
img = img.convert('L')

# Saving the greyscale image
img.save("greyscale.pgm")