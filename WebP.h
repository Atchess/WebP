#ifndef _WEBP_H_
#define _WEBP_H_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <math.h>

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
    Mat img_origin, img_16base;
    Mat dct_Y, dct_U, dct_V;

    Mat img_reconstruct, img_reconstruct_16base;
    Mat idct_Y, idct_U, idct_V;
    Mat Y_reconstruct, U_reconstruct, V_reconstruct; 
    unsigned int block_rows, block_cols, height, width;
    vector<unsigned int> predict_type;
    vector<unsigned int> reconstruct_type;

    String path;

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
    Mat inzigzag(Mat input);
    Mat indct(int block_num, int channel);
    Mat inpredictCoding(int block_num, int block_size, Mat channel_mat, Mat residual_mat);
    Mat inhPredict(int block_num, int block_size, Mat channel_mat, Mat residual_mat);
    Mat invPredict(int block_num, int block_size, Mat channel_mat, Mat residual_mat);
    Mat indcPredict(int block_num, int block_size, Mat channel_mat, Mat residual_mat);
    Mat intmPredict(int block_num, int block_size, Mat channel_mat, Mat residual_mat);
    Mat reconstructImage();
    void uncompress();

    void test();

    // write and read
    void writeData();
    void readData();
public:
    WebP();
    WebP(String path);
    ~WebP();
}; 


#endif