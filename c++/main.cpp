#include <QCoreApplication>

#include <iostream>

#include "vision.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    Vision camera("udpsrc port=5000 ! application/x-rtp, encoding-name=H264, payload=96 ! rtph264depay ! avdec_h264 ! videoconvert ! appsink");

    while(1)
    {
        camera.activate();
        camera.analyse();
        // Press  ESC on keyboard to exit
        char c=(char)cv::waitKey(25);
        if(c==27)
          return -1;
    }

    return a.exec();
}
