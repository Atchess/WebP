#include "WebP.h"

WebP::WebP(String path) {
    imagePretreatment(path);
    subSampling();
    imshow("origin", img_origin);
    imshow("16", img_16base);
}

WebP::~WebP() {

}

void WebP::compress() {
    for (int i = 0; i < block_rows; i ++) {
        for (int j = 0; j < block_cols; j ++) {
            
        }
    }
}

Mat WebP::zigzag(Mat input) {
    Mat zigMat = Mat::zeros(input.cols * input.rows, 0, CV_16S);
    zigMat.at<short>(0,0) = input.at<short>(0,0);
    int i = 0, j = 1, k = 1;
    int flag = 1, direction = 0;
    while (1) {
        if (i == input.rows - 1 && j == input.cols - 1) {
            zigMat.at<short>(0, k) = input.at<short>(input.rows - 1, input.cols - 1);
            break;
        }
        if (i == 0) {
            if (flag == 0) {
                if (i != input.rows - 1) {
                    flag = 1;
                    zigMat.at<short>(0, k ++) = input.at<short>(i, j ++);
                    continue;
                }
            } else {
                flag = 0;
                direction = 0;
                zigMat.at<short>(0, k ++) = input.at<short>(i ++, j --);
                continue;
            }
        }
        if (j == 0) {
            if (flag == 0) {
                if (j != input.cols - 1) {
                    flag = 1;
                    zigMat.at<short>(0, k ++) = input.at<short>(i ++, j);
                    continue;
                }
            } else {
                flag = 0;
                direction = 1;
                zigMat.at<short>(0, k ++) = input.at<short>(i --, j ++);
                continue;
            }
        }
        if (i == input.rows - 1) {
            if (flag == 0) {
                flag = 1;
                zigMat.at<short>(0, k ++) = input.at<short>(i, j ++);
                continue;
            } else {
                flag = 0;
                direction = 1;
                zigMat.at<short>(0, k ++) = input.at<short>(i --, j ++);
                continue;
            }
        }
        if (j == input.cols - 1) {
            if (flag == 0) {
                flag = 1;
                zigMat.at<short>(0, k ++) = input.at<short>(i ++, j);
                continue;
            } else {
                flag = 0;
                direction = 0;
                zigMat.at<short>(0, k ++) = input.at<short>(i ++, j --);
                continue;
            }
        }
        if (direction) {
            zigMat.at<short>(0, k ++) = input.at<short>(i --, j ++);
        } else {
            zigMat.at<short>(0, k ++) = input.at<short>(i ++, j --);
        }
    }
    return zigMat;
}

void WebP::imagePretreatment(String path) {
    img_origin = imread(path);
    Mat img_YUV, img_temp1, img_temp2;
    int cols, rows;
    cvtColor(img_origin, img_YUV, COLOR_RGB2YUV);
    cols = img_origin.cols % MACROBLOCKSIZE == 0 ? img_origin.cols : (img_origin.cols / MACROBLOCKSIZE + 1) * MACROBLOCKSIZE;
    rows = img_origin.rows % MACROBLOCKSIZE == 0 ? img_origin.rows : (img_origin.rows / MACROBLOCKSIZE + 1) * MACROBLOCKSIZE;
    img_16base.create(rows, cols, CV_16SC3);
    img_temp1 = img_origin;
    img_temp2 = img_16base(Range(0, img_origin.rows), Range(0, img_origin.cols));
    img_temp1.copyTo(img_temp2);
    imshow("test", img_temp2);
    imshow("te", img_YUV);
    if (img_YUV.cols % MACROBLOCKSIZE != 0) {
        int patch = MACROBLOCKSIZE - img_YUV.cols % MACROBLOCKSIZE;
        /*
        for (int i = 0; i < img_16base.rows; i++) {
            for (int j = 0; j < patch; j++) {
                img_16base.at<Vec3s>(i, img_origin.cols + j)[0] = img_origin.at<Vec3s>(i, img_origin.cols - 1)[0];
                img_16base.at<Vec3s>(i, img_origin.cols + j)[1] = img_origin.at<Vec3s>(i, img_origin.cols - 1)[1];
                img_16base.at<Vec3s>(i, img_origin.cols + j)[2] = img_origin.at<Vec3s>(i, img_origin.cols - 1)[2];
            }
        }
        */
        img_temp1 = img_origin(Range(0, img_origin.rows), Range(img_origin.cols - 1, img_origin.cols));
        for (int i = 0; i < patch; i++) {
            img_temp2 = img_16base(Range(0, img_origin.rows), Range(img_origin.cols + i, img_origin.cols + i + 1));
            img_temp1.copyTo(img_temp2);
        }
       
    }
    if (img_YUV.rows % MACROBLOCKSIZE != 0) {
        int patch = MACROBLOCKSIZE - img_YUV.rows % MACROBLOCKSIZE;
        img_temp1 = img_16base(Range(img_origin.rows - 1, img_origin.rows), Range(0, img_16base.cols));
        for (int i = 0; i < patch; i++) {
            img_temp2 = img_16base(Range(img_origin.rows + i, img_origin.rows + i + 1), Range(0, img_16base.cols));
            img_temp1.copyTo(img_temp2);
        }
    }

    block_cols = img_16base.cols / MACROBLOCKSIZE;
    block_rows = img_16base.rows / MACROBLOCKSIZE;
}


void WebP::subSampling() {
    Y.create(img_16base.rows, img_16base.cols, CV_16S);
    U.create(img_16base.rows / 2, img_16base.cols / 2, CV_16S);
    V.create(img_16base.rows / 2, img_16base.cols / 2, CV_16S);
    for (int i = 0; i < img_16base.rows / 2; i++) {
        for (int j = 0; j < img_16base.cols / 2; j++) {
            U.at<short>(i, j) = (img_16base.at<Vec3s>(i * 2, j * 2)[1] + img_16base.at<Vec3s>(i * 2 + 1, j * 2)[1] + img_16base.at<Vec3s>(i * 2, j * 2 + 1)[1] + img_16base.at<Vec3s>(i * 2 + 1, j * 2 + 1)[1]) / 4;
            V.at<short>(i, j) = (img_16base.at<Vec3s>(i * 2, j * 2)[2] + img_16base.at<Vec3s>(i * 2 + 1, j * 2)[2] + img_16base.at<Vec3s>(i * 2, j * 2 + 1)[2] + img_16base.at<Vec3s>(i * 2 + 1, j * 2 + 1)[2]) / 4;
            Y.at<short>(i * 2, j * 2) = img_16base.at<Vec3s>(i * 2, j * 2)[0];
            Y.at<short>(i * 2 + 1, j * 2) = img_16base.at<Vec3s>(i * 2 + 1, j * 2)[0];
            Y.at<short>(i * 2, j * 2 + 1) = img_16base.at<Vec3s>(i * 2, j * 2 + 1)[0];
            Y.at<short>(i * 2 + 1, j * 2 + 1) = img_16base.at<Vec3s>(i * 2 + 1, j * 2 + 1)[0];
        }
    }
}

Mat WebP::predictiveCoding(int block_num, int channel) {
    Mat temp0, temp1, temp2, temp3, predict_mat;
    temp0 = hPredict(block_num, channel);
    predict_mat = temp0;
    return predict_mat;
}

Mat WebP::hPredict(int block_num, int channel) {
    int block_size = MACROBLOCKSIZE; 
    Mat * channel_mat;
    switch (channel) {
    case 0:
        channel_mat = &Y;
        break;
    case 1:
        channel_mat = &U;
        block_size /= 2; 
        break;
    case 2:
        channel_mat = &V;
        block_size /= 2; 
        break;
    default:
        cout<<"[ERROR]: wrong channel!"<<endl;
        exit(-1);
        break;
    }
    Mat predict_mat;
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    predict_mat.zeros(block_size, block_size, CV_16S);
    if (block_col != 0) {
        Mat temp = (*channel_mat)(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size - 1, block_col * block_size + block_size - 1));
        temp.copyTo(predict_mat);
    } else {
        Mat temp0 = (*channel_mat)(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size - 1));
        Mat temp1 = predict_mat(Range(0, block_size), Range(1, block_size));
        temp0.copyTo(temp1);
    }
    return predict_mat;
}