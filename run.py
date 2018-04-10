#! /usr/bin/python

import os, sys
from PIL import Image

name = sys.argv[1]
image = Image.open(name)
image.save("./outputs/images/original.png")
os.chdir("./src")

os.chdir("./ColoredToGrayscale")
os.system("python coloredToGrayscale.py")
os.chdir("../")

os.chdir("./EdgeFeatureDetection")
os.system("make")
os.system("./EDTest.out")
os.chdir("../")

os.chdir("./DouglasPeucker")
os.system("g++ DouglasPeucker.cpp -o DouglasPeucker.out")
os.system("./DouglasPeucker.out")
os.chdir("../")

os.chdir("./SaliencyDetection")
os.system("python saliency.py")
os.chdir("../")

os.chdir("./FeatureFlowField")
os.system("g++ FeatureFlowField.cpp -o FeatureFlowField.out")
os.system("./FeatureFlowField.out")
os.chdir("../")
