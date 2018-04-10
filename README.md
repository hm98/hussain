# hussain
Artistic Low Poly rendering for Images

sudo apt install docker.io  
sudo systemctl restart docker.service  
sudo docker pull ubuntu:16.04  
sudo docker run -v /path/to/hussain/:/hussain -it ubuntu:16.04 /bin/bash  
cd hussain/ 
chmod +x install.sh   
chmod +x run.py
./install.sh
./run.py <IMAGE_NAME(JPG or PNG)>

*Open a new Terminal*  
docker ps  
sudo docker commit <CONTAINER ID> ubuntu:hussain  
sudo docker image ls  
Here you should see REPOSITORY:ubuntu TAG:hussain  


### To run the container again  
sudo docker run -v /Users/hm_98/Documents/Projects/hussain/:/hussain -it ubuntu:hussain /bin/bash   
