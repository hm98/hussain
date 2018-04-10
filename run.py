#! /usr/bin/python

import os, sys
from PIL import Image

name = sys.argv[1]
image = Image.open(name)
image.save("./outputs/images/original.png")
image.close()
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

os.chdir("./VertexOptimisation")
os.system("g++ VertexOptimisation.cpp -o VertexOptimisation.out")
os.system("g++ IOFormat.cpp -o IOFormat.out")
os.system("./VertexOptimisation.out")
os.system("./IOFormat.out")
os.chdir("../")

os.chdir("../outputs/images")
image = Image.open("DistanceMap.png")
image.save("a.png")
image.close()
os.system("mv a.png DistanceMap.png")
image = Image.open("FeatureMap.png")
image.save("a.png")
image.close()
os.system("mv a.png FeatureMap.png")
image = Image.open("RandomSeeds.png")
image.save("a.png")
image.close()
os.system("mv a.png RandomSeeds.png")
image = Image.open("OptimisedSeeds.png")
image.save("a.png")
image.close()
os.system("mv a.png OptimisedSeeds.png")
os.chdir("../../src")

os.chdir("./ConstrainedDelaunay")
os.system("make")
os.system("./triangle -p ../../outputs/data/triangle")
os.system("./showme ../../outputs/data/triangle")
