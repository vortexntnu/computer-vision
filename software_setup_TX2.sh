#!/bin/bash

# ***
# Remove all previous installations of OpenCV
# and install necessary dependencies.
#
# ***

# Colors
RD='\033[0;31m' # Red
BL='\033[0;34m' # Blue
NC='\033[0m'    # No Color

#OpenCV version
IN_DIR="${HOME}/src"
OPENCV_V="3.4.0"

# Path
ENV_NAME='tx2-env'


# Check if sudo
if [ "$EUID" -ne 0 ]
  then printf "${RD}[!] Please run as root (sudo).${NC}\n"
  exit
fi

clear

# --- Update after Jetpack install ---

sudo apt-get update
sudo apt-get upgrade


# --- OpenCV install preparations ---

printf "${BL}[i] OpenCV install preparations:${NC}\n"

# --- Remove old versions ---

printf "${BL}[i] Removing old versions:${NC}\n"

# Remove all OpenCV files installed by JetPack
sudo apt-get purge libopencv*
# Remove apt-get version of numpy
sudo apt-get purge python-numpy
# Clean up unused apt packages
sudo apt autoremove
# Upgrade all apt packages to latest version (optional)
sudo apt-get update
sudo apt-get dist-upgrade

# Update gcc apt package to the latest version (highly recommended)
printf "${BL}[i] Updating compiler:${NC}\n"
sudo apt-get install -y --only-upgrade g++-5 cpp-5 gcc-5

# --- Install dependencies ---

printf "${BL}[i] Installing relevant dependencies:${NC}\n"

# Install dependencies based on the Jetson "Installing OpenCV Guide"
printf "${BL}[i] (1/5) From Jetson OpenCV Guide:${NC}\n"
sudo apt-get install -y build-essential make cmake cmake-curses-gui \
	g++ libavformat-dev libavutil-dev \
	libswscale-dev libv4l-dev libeigen3-dev \
	libglew-dev libgtk2.0-dev

# Install dependencies for GStreamer
printf "${BL}[i] (2/5) For GStreamer:${NC}\n"
sudo apt-get install -y libdc1394-22-dev libxine2-dev \
	libgstreamer1.0-dev \
	libgstreamer-plugins-base1.0-dev

# Install additional dependencies based on pyimageresearch article
printf "${BL}[i] (3/5) For python imaging:${NC}\n"
sudo apt-get install -y libjpeg8-dev libjpeg-turbo8-dev libtiff5-dev \
	libjasper-dev libpng12-dev libavcodec-dev
sudo apt-get install -y libxvidcore-dev libx264-dev libgtk-3-dev \
	libatlas-base-dev gfortran

# Install Qt5 dependencies (needed for OpenGL support)
printf "${BL}[i] (4/5) For OpenGL:${NC}\n"
sudo apt-get install -y qt5-default

# Install dependencies for python2
printf "${BL}[i] (5/5) For python 2:${NC}\n"
sudo apt-get install -y python-dev python-pip python-tk
#sudo pip install numpy

# Install dependencies for python 3 (optional)
sudo apt-get install python3-dev python3-pip python3-tk
sudo pip3 install numpy


# --OpenCV install ---

printf "${BL}[i] OpenCV ${OPENCV_V} environment init:${NC}\n"

# --- Install directory ---

printf "${BL}[i] (1/3) Making source dir '${IN_DIR}':${NC}\n"

mkdir ${IN_DIR}
cd ${IN_DIR}

# --- Download OpenCV source ---

printf "{BL}[i] Installing packages for OpenCV:${NC}\n"
sudo apt-get install libeigen3-dev libgflags-dev libgoogle-glog-dev libhdf5-dev 

printf "${BL}[i] (2/3) Dowloading OpenCV source v${OPENCV_V}:${NC}\n"

wget https://www.github.com/opencv/opencv/archive/${OPENCV_V}.zip -O opencv-${OPENCV_V}.zip
unzip opencv-${OPENCV_V}.zip
wget https://www.github.com/opencv/opencv_contrib/archive/${OPENCV_V}.zip -O opencv_contrib-${OPENCV_V}.zip
unzip opencv_contrib-${OPENCV_V}.zip
sudo mv  -v ~/opencv_contrib-${OPENCV_V} ~/opencv-${OPENCV_V}

# --- Build OpenCV ---
cd ${IN_DIR}/opencv-${OPENCV_V}
mkdir build
cd build

# May need to use sudo for the following
sudo cmake -DOPENCV_EXTRA_MODULES_PATH=/home/nvidia/opencv-${OPENCV_V}/opencv_contrib-${OPENCV_V}/modules /home/nvidia/opencv-${OPENCV_V}
sudo make -j5
sudo make install
cd

# --- Info ---

printf "${BL}[i] To build and install OpenCV, run:${NC}\n"
printf "${GR}    cd ${PWD}${NC}\n"
printf "${GR}    make -j4${NC}\n"
printf "${GR}    make install${NC}\n"


# --- Setuo Virtual enviroment ---

printf "${BL}[i] Setting up virtual environment '${ENV_NAME}':${NC}\n"

# -- Dependencies --

printf "${BL}[i] (1/3) Installing dependencies:${NC}\n" 

sudo apt-get install libopencv-dev python-opencv
sudo apt-get install python-pip python-dev python-virtualenv

# -- Environment --

printf "${BL}[i] (2/3) Setting up environment in ~/${ENV_NAME}:${NC}\n"

mkdir ~/${ENV_NAME}
virtualenv ~/${ENV_NAME}
# Activate
source ~/${ENV_NAME}/bin/activate

# -- Requirements --

printf "${BL}[i] (3/3) Installing requirements:${NC}\n"
if [ -e requirements.txt ]
then
    sudo pip install -r requirements.txt
else
    printf "${RD}[!] Requirements not installed: File not found.${NC}\n"
fi

# -- Info --
printf "${BL}[i] Activate virtual environment:\n"
printf "${GR}    source ~/${ENV_NAME}/bin/activate${NC}\n"
printf "${BL}[i] Deactivate virtual environment:${NC}\n"
printf "${GR}    deactivate${NC}\n"



# --- Installing ROS ---

printf "{BL}[i]Installing ROS:${NC}\n"

sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
sudo apt-key adv --keyserver hkp://ha.pool.sks-keyservers.net:80 --recv-key 421C365BD9FF1F717815A3895523BAEEB01FA116
sudo apt-get update
sudo apt-get install ros-kinetic-desktop
sudo rosdep init
rosdep update
echo "source /opt/ros/kinetic/setup.bash" >> ~/.bashrc
source ~/.bashrc
sudo apt-get install python-rosinstall python-rosinstall-generator python-wstool build-essential


# -- Get LibWebsockets --

printf "{BL}[i]Installing LibWebsockets:${NC}\n"

wget https://github.com/warmcat/libwebsockets/archive/1.7.1.zip -O libwebsockets_1.7.1.zip
unzip libwebsockets_1.7.1.zip
cd libwebsockets_1.7.1
mkdir build
cd build
sudo cmake ..
sudo make
sudo make install
ldconfig
cd

# -- Get Nginx-server --

printf "{BL}[i]Installing Nginx-server:${NC}\n"

sudo apt-get install build-essential libpcre3 libpcre3-dev libssl-dev
wget http://nginx.org/download/nginx-1.6.2.tar.gz
wget https://github.com/arut/nginx-rtmp-module/archive/master.zip
tar -zxvf nginx-1.6.2.tar.gz
unzip master.zip
cd nginx-1.6.2
./configure --with-http_ssl_module --add-module=../nginx-rtmp-module-master
sudo make
sudo make install
cd ..
rm -R nginx-1.6.2
cd


# --- Install necessary packages for Gstreamer ---

printf "{BL}[i]Adding necessary packages for Gstreamer:${NC}\n"

sudo apt-get install libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0 -plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools

printf "{BL}[i]Installation done:${NC}\n"

