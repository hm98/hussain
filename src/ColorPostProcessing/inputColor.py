#! /usr/bin/python

import cv2, os

outputFile = open("../../outputs/data/originalColors.data", "w+")
image = cv2.imread("../../outputs/images/original.png")

height, width, _ = image.shape

outputFile.write(str(width) + " " + str(height) + "\n")

for i in range(0, width):
		for j in range(0, height):
			outputFile.write(str(image.item(j,i,0)) + " " + str(image.item(j,i,1)) + " " + str(image.item(j,i,2)) + " ")
		outputFile.write("\n")

outputFile.close()

os.system("g++ colorTriangle.cpp -o colorTriangle.out")
os.system("./colorTriangle.out")

cnt = 0

inputFile = open("../../outputs/data/finalColors.data")
for line in inputFile:
	values = line.split(" ")
	for j in range(0, height):
		for k in range (0, 3):
			image.itemset((j, cnt, k), values[j * 3 + k])
	cnt += 1

cv2.imwrite("../../outputs/images/final.png", image)
inputFile.close()
