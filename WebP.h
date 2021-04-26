#ifndef _WEBP_H_
#define _WEBP_H_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "MyRange.h"

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

#define BIT67(ch) ((unsigned int)((192 & ch) >> 6))
#define BIT45(ch) ((unsigned int)((48 & ch) >> 4))
#define BIT23(ch) ((unsigned int)((12 & ch) >> 2))
#define BIT01(ch) ((unsigned int)(3 & ch))


class WebP {
private:
    Mat Y, U, V;
    Mat img_origin, img_16base;
    Mat dct_Y, dct_U, dct_V;
    Mat dct_mat_16, dct_mat_8, dct_mat_16_T, dct_mat_8_T;

    Mat img_reconstruct, img_reconstruct_16base;
    Mat quantizationTable_Y, quantizationTable_UV, quantizationTable_Y_dc, quantizationTable_UV_dc;
    Mat idct_Y, idct_U, idct_V;
    Mat Y_reconstruct, U_reconstruct, V_reconstruct; 
    unsigned int block_rows, block_cols, height, width, _pre_type;
    vector<unsigned int> predict_type;
    vector<unsigned int> reconstruct_type;

    String path, filename;
    ofstream outfile;
    ifstream infile;

    map<short, MyRange> arithmeticMap_Y, arithmeticMap_U, arithmeticMap_V;
    vector<short> vec_DAR_Y, vec_DAR_U, vec_DAR_V;
    int lenBeforeArith_Y, lenBeforeArith_U, lenBeforeArith_V;
    double res_Y, res_U, res_V;

    vector<short> ivec_DAR_Y, ivec_DAR_U, ivec_DAR_V;

    // dct
    void initDctMat();
    Mat mydct(Mat input_mat);
    Mat myidct(Mat input_mat);

    // compress
    void imagePretreatment();
    void compress();
    void subSampling();
    void initQuantizationTable();
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
    Mat inpredictCoding(int block_num, int block_size, int channel, Mat residual_mat);
    Mat inhPredict(int block_num, int block_size, Mat channel_mat, Mat residual_mat);
    Mat invPredict(int block_num, int block_size, Mat channel_mat, Mat residual_mat);
    Mat indcPredict(int block_num, int block_size, Mat channel_mat, Mat residual_mat);
    Mat intmPredict(int block_num, int block_size, Mat channel_mat, Mat residual_mat);
    Mat reconstructImage();
    void decompress();

    void test();

    vector<short> DPCMAndRim(Mat mat);
    void DPCMAndRimCoding();

    Mat deDPCMAndRim(vector<short> vec, int channel);
    void deDPCMAndRimCoding();
    
    double arithmetic(vector<short> vec,map<short, MyRange>* arithmeticMap, int* lenBeforeArith);
    void arithmeticCoding();

    vector<short> deArithmetic(double code, int channel);
    void deArithmeticCoding();


    // write and read
    void writeData();
    void writeTypeData();
    void writeArithmeticCode();
    void writeRunLengthCode();
    void readData();
    void readTypeData();
    void readArithmeticCode();
    void readRunLengthCode();
public:
    WebP();
    WebP(String path);
    void Compress();
    void DeCompress();
    void Start();
    ~WebP();
}; 


#endif