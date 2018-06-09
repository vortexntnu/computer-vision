#ifndef VISION_H
#define VISION_H

#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"

typedef struct {
    int hsv_low[3];
    int hsv_high[3];
} hsv_colors; //blue, red1, red2, yellow;

class Vision
{
public:
    Vision(const char* pipeline);
    ~Vision();
    bool activate();
    void analyse();
    //cv::Mat create_mask();
private:
    cv::VideoCapture m_cap;
    cv::Mat m_frame;
    cv::Mat a_frame;
    hsv_colors blue;
    hsv_colors red1;
    hsv_colors red2;
    hsv_colors yellow;
};

#endif // VISION_H
