#ifndef _WEBP_H_
#define _WEBP_H_

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

class WebP {
private:
    Mat *Y, *U, *V;
    Mat *img_origin;
    void imagePretreatment(String path);

public:
    WebP();
    WebP(String path);
    ~WebP();
}; 
WebP::WebP(){} 
WebP::~WebP(){}


#endif