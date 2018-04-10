#!/bin/bash

apt update  
apt install build-essential 
apt install python 
apt install python-pip 
pip install --upgrade pip 
pip install Pillow 
apt install python-opencv
chmod +x run.py
cd lib/ 
cp libopencv_* /usr/lib/ 
cp libopencv_* /lib/ 
cp -r ILM/ /usr/include/ILM/