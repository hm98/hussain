#!/bin/bash

apt update  
apt install build-essential 
apt install python 
apt install curl 
curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py 
python get-pip.py 
rm -rf get-pip.py 
pip install Pillow 
apt install python-opencv
apt install libx11-dev
chmod +x run.py
cd lib/ 
cp libopencv_* /usr/lib/ 
cp libopencv_* /lib/ 
cp -r ILM/ /usr/include/ILM/