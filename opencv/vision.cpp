#include "vision.h"
#include <iostream>
#include <math.h>
#include <exception>


Vision::Vision(const char* pipeline)
{
    m_cap.open(pipeline);
    if(!m_cap.isOpened())
    {
        std::cout << "Failed to open camera" << std::endl;
    }

    blue = {
            .hsv_low = {100, 120, 113},
            .hsv_high = {120, 255, 255}
    };

    red1 = {
            .hsv_low = {175,100,80},
            .hsv_high = {180, 255, 255}
    };
    red2 = {
            .hsv_low = {0,100,80},
            .hsv_high = {5, 255, 255}
    };
    yellow = {
            .hsv_low = {25,80, 80},
            .hsv_high = {35, 255, 255}
    };
}

Vision::~Vision()
{
    m_cap.release();
    cv::destroyAllWindows();
}

bool Vision::activate()
{
    m_cap >> m_frame;

    if (m_frame.empty())
        return false;

    cv::imshow("original", m_frame);
    return true;
}

void Vision::analyse()
{
    m_cap >> m_frame;
    cv::Mat blurred;
    cv::GaussianBlur(m_frame, blurred, cv::Size(15,15), 0, 0);
    cv::Mat hsv;
    cv::cvtColor(blurred,hsv,CV_BGR2HSV);
    cv::Mat maskb, maskr1, maskr2, maskr, masky;
    cv::Scalar minBlue = cv::Scalar(blue.hsv_low[0], blue.hsv_low[1], blue.hsv_low[2]);
    cv::Scalar maxBlue = cv::Scalar(blue.hsv_high[0], blue.hsv_high[1], blue.hsv_high[2]);
    cv::inRange(hsv, minBlue, maxBlue, maskb);

    int mask_width = 640;
    int mask_height = 480;
    cv::namedWindow( "Blue mask",CV_WINDOW_NORMAL);
    cv::resizeWindow("Display frame", mask_width, mask_height);
    cv::imshow("Blue mask", maskb);

    //sorting contours of the mask
    std::vector<std::vector<cv::Point> > blue_contours;
    std::vector<std::vector<cv::Point> > blue_contours_area;
    std::vector<cv::Vec4i> hierarchy_b;
    cv::findContours(maskb, blue_contours, hierarchy_b, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0,0));
    // Cutting away contours that are too small
    double min_area, max_area;
    min_area = 500;
    max_area = 200000;
    for(unsigned int i = 0; i<blue_contours.size(); i=i+1)
    {
        if(min_area < cv::contourArea(blue_contours[i]) && cv::contourArea(blue_contours[i]) < max_area)
            blue_contours_area.push_back(blue_contours[i]);
    }
    // Sorting the approved areas to identify squares and triangles
    std::vector<std::vector<cv::Point> > blue_triangles;
    std::vector<std::vector<cv::Point> > blue_squares;
    double perimeter, area, circularity;
    double circ_tri_low, circ_tri_high, circ_squ_low, circ_squ_high;
    circ_tri_low = 0.4;
    circ_tri_high = 0.6;
    circ_squ_low = 0.6;
    circ_squ_high = 0.8;
    for(unsigned int i = 0; i<blue_contours_area.size(); i=i+1)
    {
        perimeter = cv::arcLength(blue_contours_area[i],true);
        if(perimeter != 0)
        {
            area = cv::contourArea(blue_contours_area[i]);
            circularity = (4*3.141592653589793238463*area)/(perimeter*perimeter);
            circularity = 1 - circularity;
            if(circ_tri_low < circularity && circularity < circ_tri_high)
                blue_triangles.push_back(blue_contours_area[i]);
            if(circ_squ_low < circularity && circularity < circ_squ_high)
                blue_squares.push_back(blue_contours_area[i]);
        }
    }

    // Display bounding boxes
    // Triangles
    std::vector<cv::Moments> mu_b(blue_triangles.size());
    std::vector<cv::Point2f> mc_b( blue_triangles.size());
    std::vector<std::vector<cv::Point> > contours_poly_b( blue_triangles.size() );
    std::vector<cv::Rect> boundRect_b( blue_triangles.size() );

    for(int i = 0; i < blue_triangles.size(); i = i+1)
    {
        mu_b[i] = cv::moments(blue_triangles[i],false);
        mc_b[i] = cv::Point2f(mu_b[i].m10/mu_b[i].m00, mu_b[i].m01/mu_b[i].m00 );
    }

    // Drawing the bounding boxes
    // Triangles
    cv::Mat drawing_b = cv::Mat::zeros(maskb.size(), CV_8UC3);
    for(int i = 0; i < blue_triangles.size(); i=i+1)
    {
        cv::approxPolyDP( cv::Mat(blue_triangles[i]), contours_poly_b[i], 3, true );
        boundRect_b[i] = cv::boundingRect( cv::Mat(contours_poly_b[i]) );
        //cv::Scalar color = cv::Scalar( cv::RNG.uniform(0, 255), cv::RNG.uniform(0,255), cv::RNG.uniform(0,255) );
        cv::Scalar color = cv::Scalar( 0, 255, 0);
        cv::drawContours(m_frame, contours_poly_b, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
        cv::rectangle( m_frame, boundRect_b[i].tl(), boundRect_b[i].br(), color, 2, 8, 0);
        cv::putText(m_frame,"Blue Triangle",boundRect_b[i].tl(), cv::FONT_HERSHEY_SIMPLEX, 1, color, 2, cv::LINE_AA, false );
    }

        // Display bounding boxes
        // Rectangles
        std::vector<cv::Moments> mu_s_b(blue_squares.size());
        std::vector<cv::Point2f> mc_s_b( blue_squares.size());
        std::vector<std::vector<cv::Point> > contours_poly_s_b( blue_squares.size() );
        std::vector<cv::Rect> boundRect_s_b( blue_squares.size() );

        for(int i = 0; i < blue_squares.size(); i = i+1)
        {
            mu_s_b[i] = cv::moments(blue_squares[i],false);
            mc_s_b[i] = cv::Point2f(mu_s_b[i].m10/mu_s_b[i].m00, mu_s_b[i].m01/mu_s_b[i].m00 );
        }

        // Drawing the bounding boxe
        // Rectangles
        for(int i = 0; i < blue_squares.size(); i=i+1)
        {
            cv::approxPolyDP( cv::Mat(blue_squares[i]), contours_poly_s_b[i], 3, true );
            boundRect_s_b[i] = cv::boundingRect( cv::Mat(contours_poly_s_b[i]) );
            //cv::Scalar color = cv::Scalar( cv::RNG.uniform(0, 255), cv::RNG.uniform(0,255), cv::RNG.uniform(0,255) );
            cv::Scalar color = cv::Scalar( 0, 255, 0);
            cv::drawContours(m_frame, contours_poly_s_b, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
            try
            {
                cv::rectangle( m_frame, boundRect_s_b[i].tl(), boundRect_s_b[i].br(), color, 2, 8, 0);
                cv::putText(m_frame,"Blue Rectangle",boundRect_s_b[i].tl(), cv::FONT_HERSHEY_SIMPLEX, 1, color, 2, cv::LINE_AA, false );
                //putText(Mat& img, const string& text, Point org, int fontFace, double fontScale, Scalar color, int thickness=1, int lineType=8, bool bottomLeftOrigin=false )


            }
            catch(cv::Exception &e)
            {
                std::cout <<e.what() << std::endl;
            }
        }

    cv::Scalar minRed1 = cv::Scalar(red1.hsv_low[0], red1.hsv_low[1], red1.hsv_low[2]);
    cv::Scalar maxRed1 = cv::Scalar(red1.hsv_high[0], red1.hsv_high[1], red1.hsv_high[2]);
    cv::inRange(hsv, minRed1, maxRed1, maskr1);
    cv::Scalar minRed2 = cv::Scalar(red2.hsv_low[0], red2.hsv_low[1], red2.hsv_low[2]);
    cv::Scalar maxRed2 = cv::Scalar(red2.hsv_high[0], red2.hsv_high[1], red2.hsv_high[2]);
    cv::inRange(hsv, minRed2, maxRed2, maskr2);
    maskr = maskr1 + maskr2;

    cv::namedWindow("Red mask",CV_WINDOW_NORMAL);
    cv::resizeWindow("Display frame", mask_width, mask_height);
    cv::imshow("Red mask", maskr);

    //sorting contours of the mask
    std::vector<std::vector<cv::Point> > red_contours;
    std::vector<std::vector<cv::Point> > red_contours_area;
    std::vector<cv::Vec4i> hierarchy_r;
    cv::findContours(maskr, red_contours, hierarchy_r, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0,0));
    // Cutting away contours that are too small

    for(unsigned int i = 0; i<red_contours.size(); i=i+1)
    {
        if(min_area < cv::contourArea(red_contours[i]) && cv::contourArea(red_contours[i]) < max_area)
            red_contours_area.push_back(red_contours[i]);
    }
    // Sorting the approved areas to identify squares and triangles
    std::vector<std::vector<cv::Point> > red_triangles;
    std::vector<std::vector<cv::Point> > red_squares;

    for(unsigned int i = 0; i<red_contours_area.size(); i=i+1)
    {
        perimeter = cv::arcLength(red_contours_area[i],true);
        if(perimeter != 0)
        {
            area = cv::contourArea(red_contours_area[i]);
            circularity = (4*3.141592653589793238463*area)/(perimeter*perimeter);
            circularity = 1 - circularity;
            if(circ_tri_low < circularity && circularity < circ_tri_high)
                red_triangles.push_back(red_contours_area[i]);
            if(circ_squ_low < circularity && circularity < circ_squ_high)
                red_squares.push_back(red_contours_area[i]);
        }
    }

    // Display bounding boxes
    // Triangles
    std::vector<cv::Moments> mu_r(red_triangles.size());
    std::vector<cv::Point2f> mc_r( red_triangles.size());
    std::vector<std::vector<cv::Point> > contours_poly_r( red_triangles.size() );
    std::vector<cv::Rect> boundRect_r( red_triangles.size() );

    for(int i = 0; i < red_triangles.size(); i = i+1)
    {
        mu_r[i] = cv::moments(red_triangles[i],false);
        mc_r[i] = cv::Point2f(mu_r[i].m10/mu_r[i].m00, mu_r[i].m01/mu_r[i].m00 );
    }

    // Drawing the bounding boxes
    // Triangles
    cv::Mat drawing_r = cv::Mat::zeros(maskb.size(), CV_8UC3);
    for(int i = 0; i < red_triangles.size(); i=i+1)
    {
        cv::approxPolyDP( cv::Mat(red_triangles[i]), contours_poly_r[i], 3, true );
        boundRect_r[i] = cv::boundingRect( cv::Mat(contours_poly_r[i]) );
        //cv::Scalar color = cv::Scalar( cv::RNG.uniform(0, 255), cv::RNG.uniform(0,255), cv::RNG.uniform(0,255) );
        cv::Scalar color = cv::Scalar( 0, 255, 0);
        cv::drawContours(m_frame, contours_poly_r, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
        cv::rectangle( m_frame, boundRect_r[i].tl(), boundRect_r[i].br(), color, 2, 8, 0);
        cv::putText(m_frame,"Red Triangle",boundRect_r[i].tl(), cv::FONT_HERSHEY_SIMPLEX, 1, color, 2, cv::LINE_AA, false );
    }

        // Display bounding boxes
        // Rectangles
        std::vector<cv::Moments> mu_s_r(red_squares.size());
        std::vector<cv::Point2f> mc_s_r( red_squares.size());
        std::vector<std::vector<cv::Point> > contours_poly_s_r( red_squares.size() );
        std::vector<cv::Rect> boundRect_s_r( red_squares.size() );

        for(int i = 0; i < red_squares.size(); i = i+1)
        {
            mu_s_r[i] = cv::moments(red_squares[i],false);
            mc_s_r[i] = cv::Point2f(mu_s_r[i].m10/mu_s_r[i].m00, mu_s_r[i].m01/mu_s_r[i].m00 );
        }

        // Drawing the bounding boxe
        // Rectangles
        for(int i = 0; i < red_squares.size(); i=i+1)
        {
            cv::approxPolyDP( cv::Mat(red_squares[i]), contours_poly_s_r[i], 3, true );
            boundRect_s_r[i] = cv::boundingRect( cv::Mat(contours_poly_s_r[i]) );
            //cv::Scalar color = cv::Scalar( cv::RNG.uniform(0, 255), cv::RNG.uniform(0,255), cv::RNG.uniform(0,255) );
            cv::Scalar color = cv::Scalar( 0, 255, 0);
            cv::drawContours(m_frame, contours_poly_s_r, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
            try
            {
                cv::rectangle( m_frame, boundRect_s_r[i].tl(), boundRect_s_r[i].br(), color, 2, 8, 0);
                cv::putText(m_frame,"Red Rectangle",boundRect_s_r[i].tl(), cv::FONT_HERSHEY_SIMPLEX, 1, color, 2, cv::LINE_AA, false );
                //putText(Mat& img, const string& text, Point org, int fontFace, double fontScale, Scalar color, int thickness=1, int lineType=8, bool bottomLeftOrigin=false )


            }
            catch(cv::Exception &e)
            {
                std::cout <<e.what() << std::endl;
            }
        }


    cv::Scalar minYellow = cv::Scalar(yellow.hsv_low[0], yellow.hsv_low[1], yellow.hsv_low[2]);
    cv::Scalar maxYellow = cv::Scalar(yellow.hsv_high[0], yellow.hsv_high[1], yellow.hsv_high[2]);
    cv::inRange(hsv, minYellow, maxYellow, masky);

    cv::namedWindow("Yellow mask",CV_WINDOW_NORMAL);
    cv::resizeWindow("Display frame", mask_width, mask_height);
    cv::imshow("Yellow mask", masky);

    //sorting contours of the mask
    std::vector<std::vector<cv::Point> > yellow_contours;
    std::vector<std::vector<cv::Point> > yellow_contours_area;
    std::vector<cv::Vec4i> hierarchy_y;
    cv::findContours(masky, yellow_contours, hierarchy_y, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0,0));
    // Cutting away contours that are too small

    for(unsigned int i = 0; i<yellow_contours.size(); i=i+1)
    {
        if(min_area < cv::contourArea(yellow_contours[i]) && cv::contourArea(yellow_contours[i]) < max_area)
            yellow_contours_area.push_back(yellow_contours[i]);
    }
    // Sorting the approved areas to identify squares and triangles
    std::vector<std::vector<cv::Point> > yellow_triangles;
    std::vector<std::vector<cv::Point> > yellow_squares;

    for(unsigned int i = 0; i<yellow_contours_area.size(); i=i+1)
    {
        perimeter = cv::arcLength(yellow_contours_area[i],true);
        if(perimeter != 0)
        {
            area = cv::contourArea(yellow_contours_area[i]);
            circularity = (4*3.141592653589793238463*area)/(perimeter*perimeter);
            circularity = 1 - circularity;
            if(circ_tri_low < circularity && circularity < circ_tri_high)
                yellow_triangles.push_back(yellow_contours_area[i]);
            if(circ_squ_low < circularity && circularity < circ_squ_high)
                yellow_squares.push_back(yellow_contours_area[i]);
        }
    }

    // Display bounding boxes
    // Triangles
    std::vector<cv::Moments> mu_y(yellow_triangles.size());
    std::vector<cv::Point2f> mc_y( yellow_triangles.size());
    std::vector<std::vector<cv::Point> > contours_poly_y( yellow_triangles.size() );
    std::vector<cv::Rect> boundRect_y( yellow_triangles.size() );

    for(int i = 0; i < yellow_triangles.size(); i = i+1)
    {
        mu_y[i] = cv::moments(yellow_triangles[i],false);
        mc_y[i] = cv::Point2f(mu_y[i].m10/mu_y[i].m00, mu_y[i].m01/mu_y[i].m00 );
    }

    // Drawing the bounding boxes
    // Triangles
    cv::Mat drawing_y = cv::Mat::zeros(maskb.size(), CV_8UC3);
    for(int i = 0; i < yellow_triangles.size(); i=i+1)
    {
        cv::approxPolyDP( cv::Mat(yellow_triangles[i]), contours_poly_y[i], 3, true );
        boundRect_y[i] = cv::boundingRect( cv::Mat(contours_poly_y[i]) );
        //cv::Scalar color = cv::Scalar( cv::RNG.uniform(0, 255), cv::RNG.uniform(0,255), cv::RNG.uniform(0,255) );
        cv::Scalar color = cv::Scalar( 0, 255, 0);
        cv::drawContours(m_frame, contours_poly_y, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
        cv::rectangle( m_frame, boundRect_y[i].tl(), boundRect_y[i].br(), color, 2, 8, 0);
        cv::putText(m_frame,"Yellow Triangle",boundRect_y[i].tl(), cv::FONT_HERSHEY_SIMPLEX, 1, color, 2, cv::LINE_AA, false );
    }

        // Display bounding boxes
        // Rectangles
        std::vector<cv::Moments> mu_s_y(yellow_squares.size());
        std::vector<cv::Point2f> mc_s_y( yellow_squares.size());
        std::vector<std::vector<cv::Point> > contours_poly_s_y( yellow_squares.size() );
        std::vector<cv::Rect> boundRect_s_y( yellow_squares.size() );

        for(int i = 0; i < yellow_squares.size(); i = i+1)
        {
            mu_s_y[i] = cv::moments(yellow_squares[i],false);
            mc_s_y[i] = cv::Point2f(mu_s_y[i].m10/mu_s_y[i].m00, mu_s_y[i].m01/mu_s_y[i].m00 );
        }

        // Drawing the bounding boxe
        // Rectangles
        for(int i = 0; i < yellow_squares.size(); i=i+1)
        {
            cv::approxPolyDP( cv::Mat(yellow_squares[i]), contours_poly_s_y[i], 3, true );
            boundRect_s_y[i] = cv::boundingRect( cv::Mat(contours_poly_s_y[i]) );
            //cv::Scalar color = cv::Scalar( cv::RNG.uniform(0, 255), cv::RNG.uniform(0,255), cv::RNG.uniform(0,255) );
            cv::Scalar color = cv::Scalar( 0, 255, 0);
            cv::drawContours(m_frame, contours_poly_s_y, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
            try
            {
                cv::rectangle( m_frame, boundRect_s_y[i].tl(), boundRect_s_y[i].br(), color, 2, 8, 0);
                cv::putText(m_frame,"Yellow Rectangle",boundRect_s_y[i].tl(), cv::FONT_HERSHEY_SIMPLEX, 1, color, 2, cv::LINE_AA, false );
                //putText(Mat& img, const string& text, Point org, int fontFace, double fontScale, Scalar color, int thickness=1, int lineType=8, bool bottomLeftOrigin=false )


            }
            catch(cv::Exception &e)
            {
                std::cout <<e.what() << std::endl;
            }
        }

    // Show in a window
    cv::namedWindow("Analysed", CV_WINDOW_AUTOSIZE );
    cv::imshow("Analysed", m_frame);




}
