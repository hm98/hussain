# hussain
Artistic Low Poly rendering for Images

sudo apt install docker.io  
sudo systemctl restart docker.service  
sudo docker pull ubuntu:16.04  
sudo docker run -v /path/to/hussain/:/hussain -it ubuntu:16.04 /bin/bash  
apt update  
apt install build-essential  
apt install python  
apt install python-pip  
pip install --upgrade pip  
pip install Pillow  
cd hussain/  
chmod +x coloredToBw.py  
./coloredToBw.py  
cp greyscale.pgm ./EDTest/  
cd EDTest/  
make  
cp libopencv_* /usr/lib/  
cp libopencv_* /lib/  
./EDTest  
g++ Douglas-Pecker.cpp -o Douglas-Pecker    
./Douglas-Pecker  

*Open a new Terminal*  
docker ps  
sudo docker commit <CONTAINER ID> ubuntu:hussain  
sudo docker image ls  
Here you should see REPOSITORY:ubuntu TAG:hussain  


### To run the container again  
sudo docker run -v /Users/hm_98/Documents/Projects/hussain/:/hussain -it ubuntu:hussain /bin/bash   
