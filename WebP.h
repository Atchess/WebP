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

#define Y_CHANNEL 0
#define U_CHANNEL 1
#define V_CHANNEL 2

#define INF 100000
#define MACROBLOCKSIZE 16

class WebP {
private:
    Mat Y, U, V;
    Mat img_origin;
    Mat img_16base;
    Mat dct_Y;
    Mat dct_U;
    Mat dct_V;
    unsigned int block_rows, block_cols;
    vector<unsigned int> predict_type;

    // compress
    void imagePretreatment(String path);
    void compress();
    void subSampling();
    Mat zigzag(Mat input);
    Mat predictiveCoding(int block_num, int channel);
    Mat hPredict(int block_num, int block_size, Mat channel_mat);
    Mat vPredict(int block_num, int block_size, Mat channel_mat);
    Mat dcPredict(int block_num, int block_size, Mat channel_mat);
    Mat tmPredict(int block_num, int block_size, Mat channel_mat);
    Mat dct(int block_num, int channel);
    int EuclideanDistance(Mat mat1, Mat mat2);
    int residualLength(Mat mat);
    // decompress
    Mat unzigzag(Mat input);
    Mat undct(int block_num, int channel);
    Mat unpredictCoding(int block_num, int channel);
    Mat reconstructImage();
    void uncompress();

    // write and read
    void writeData();
    void readData();
public:
    WebP();
    WebP(String path);
    ~WebP();
}; 


#endif