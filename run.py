#! /usr/bin/python

import os, sys
from PIL import Image

name = sys.argv[1]
image = Image.open(name)
image.save("./outputs/images/original.png")
image.close()
os.chdir("./src")

os.chdir("./ColoredToGrayscale")
os.system("python coloredToGrayscale.py > /dev/null")
os.chdir("../")
print "Coverted to Grayscale"

os.chdir("./EdgeFeatureDetection")
os.system("make > /dev/null")
os.system("./EDTest.out > /dev/null")
os.chdir("../")
print "Edge Detection complete"

os.chdir("./DouglasPeucker")
os.system("g++ DouglasPeucker.cpp -o DouglasPeucker.out")
os.system("./DouglasPeucker.out > /dev/null")
os.chdir("../")
print "Douglas Peucker complete"

os.chdir("./SaliencyDetection")
os.system("python saliency.py  > /dev/null")
os.chdir("../")
print "Saliency detection complete"

os.chdir("./FeatureFlowField")
os.system("g++ FeatureFlowField.cpp -o FeatureFlowField.out")
os.system("./FeatureFlowField.out > /dev/null")
os.chdir("../")
print "FeatureFlow complete"

os.chdir("./VertexOptimisation")
os.system("g++ VertexOptimisation.cpp -o VertexOptimisation.out")
os.system("g++ IOFormat.cpp -o IOFormat.out")
os.system("./VertexOptimisation.out > /dev/null")
os.system("./IOFormat.out > /dev/null")
os.chdir("../")
print "VertexOptimisation complete"

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
os.system("make > /dev/null")
os.system("mv triangle triangle.out")
os.system("mv showme showme.out")
os.system("./triangle.out -p ../../outputs/data/triangle > /dev/null")
os.chdir("../")
print "Triangulation complete"

os.chdir("./ColorPostProcessing")
os.system("python inputColor.py")
os.chdir("../")
print "ColorPostProcessing complete"

print "All done"
