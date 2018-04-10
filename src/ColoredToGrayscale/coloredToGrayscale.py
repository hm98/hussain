#! /usr/bin/python

from PIL import Image

# Opening input image
img = Image.open('../../outputs/images/original.png')

# Convert to grayscale image
img = img.convert('LA')

# To be saved in PGM format
img = img.convert('L')

# Saving the grayscale image
img.save("../../outputs/images/grayscale.pgm")