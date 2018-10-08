#!/bin/bash

export OPENCV_VERSION=3.4.0
export OPENCV_DIR=/home/nvidia/opencv-$OPENCV_VERSION
export COLOR_DIR=/home/nvidia/computer_vision
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$OPENCV_DIR/build/lib

g++ -ggdb color_detection.cpp -o color_detect `pkg-config --cflags --libs opencv`

#\
#    -I$COLOR_DIR color_detection.cpp
#    -L$OPENCV_DIR/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lstdc++ -lm
#    -o color_detect

./color_detect

