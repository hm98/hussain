# Artistic Low-Poly Rendering

Based on the following paper : [Link](https://link.springer.com/article/10.1007/s00371-015-1082-2)

## Usage

#### First time :
```sh
sudo apt install docker.io  
sudo systemctl restart docker.service  
sudo docker pull ubuntu:16.04  
sudo docker run -v /path/to/hussain/:/hussain -it ubuntu:16.04 /bin/bash  
cd hussain/ 
chmod +x install.sh   
chmod +x run.py
./install.sh
./run.py <IMAGE_NAME(JPG or PNG)>
```

#### Saving the docker container before exit :

*Open a new Terminal*
```sh
docker ps  
sudo docker commit <CONTAINER ID> ubuntu:hussain
```


#### To run the container again :
```sh
sudo docker run -v /path/to/hussain/:/hussain -it ubuntu:hussain /bin/bash  
```

## Results

#### Input
![input](image.png)

#### Output
![output](outputs/images/final.png)
