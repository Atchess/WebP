#ifndef _WEBP_H_
#define _WEBP_H_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

#define H_PRED 0
#define V_PRED 1
#define DC_PRED 2
#define TM_PRED 3

#define MACROBLOCKSIZE 16
typedef Vec<int, 3> Vec3i;
typedef Vec<short, 3> Vec3s;

class WebP {
private:
    Mat Y, U, V;
    Mat img_origin;
    Mat img_16base;
    unsigned int block_rows, block_cols;
    vector<unsigned int> predict_type;
    void imagePretreatment(String path);
    void compress();
    void subSampling();
    Mat zigzag(Mat input);
    Mat predictiveCoding(int block_num, int channel);
    Mat hPredict(int block_num, int channel);
    Mat vPredict(int block_num, int channel);
    Mat dcPredict(int block_num, int channel);
    Mat tmPredict(int block_num, int channel);

public:
    WebP();
    WebP(String path);
    ~WebP();
}; 


#endif