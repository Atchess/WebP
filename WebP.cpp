#include "WebP.h"

WebP::WebP(String path) {
    imagePretreatment(path);
    subSampling();
}

WebP::~WebP() {

}

void WebP::compress() {
    dct_Y.create( block_cols * block_rows, MACROBLOCKSIZE * MACROBLOCKSIZE, CV_16S);
    dct_U.create( block_cols * block_rows, MACROBLOCKSIZE / 2 * MACROBLOCKSIZE / 2, CV_16S);
    dct_V.create( block_cols * block_rows, MACROBLOCKSIZE / 2 * MACROBLOCKSIZE / 2, CV_16S);
    Mat dct_block_Y, dct_block_U, dct_block_V, mat_temp;
    Mat zig_block_Y, zig_block_U, zig_block_V;
    int block_num;
    for (int i = 0; i < block_rows; i ++) {
        for (int j = 0; j < block_cols; j ++) {
            block_num = i * block_cols + j;
            
            dct_block_Y = dct(block_num, Y_CHANNEL);
            zig_block_Y = zigzag(dct_block_Y);
            mat_temp = dct_Y(Range(block_num, block_num + 1), Range(0, MACROBLOCKSIZE * MACROBLOCKSIZE));
            zig_block_Y.copyTo(mat_temp);
            
            dct_block_U = dct(block_num, U_CHANNEL);
            zig_block_U = zigzag(dct_block_U);
            mat_temp = dct_U(Range(block_num, block_num + 1), Range(0, MACROBLOCKSIZE / 2 * MACROBLOCKSIZE / 2));
            zig_block_U.copyTo(mat_temp);
            
            dct_block_V = dct(block_num, V_CHANNEL);
            zig_block_V = zigzag(dct_block_V);
            mat_temp = dct_V(Range(block_num, block_num + 1), Range(0, MACROBLOCKSIZE / 2 * MACROBLOCKSIZE / 2));
            zig_block_V.copyTo(mat_temp);
        }
    }

}

// the function is so bad, but i don't want to rewrite it. 
// maybe when i finish the project, i will consider rewriting it.
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
    Mat img_YUV, img_temp0, img_temp1, img_temp2;
    int cols, rows;
    cvtColor(img_origin, img_YUV, COLOR_RGB2YUV);
    cols = img_origin.cols % MACROBLOCKSIZE == 0 ? img_origin.cols : (img_origin.cols / MACROBLOCKSIZE + 1) * MACROBLOCKSIZE;
    rows = img_origin.rows % MACROBLOCKSIZE == 0 ? img_origin.rows : (img_origin.rows / MACROBLOCKSIZE + 1) * MACROBLOCKSIZE;
    img_16base.create(rows, cols, img_YUV.type());
    img_temp1 = img_YUV(Rect(0, 0, img_YUV.cols, img_YUV.rows));
    img_temp2 = img_16base(Range(0, img_YUV.rows), Range(0, img_YUV.cols));
    img_temp1.copyTo(img_temp2);
    // Mat test = Mat::zeros(img_16base.cols / 2, img_16base.rows / 2, img_YUV.type());
    // resize(img_16base, test, Size(img_16base.cols / 2, img_16base.rows / 2));
    // imshow("test1", test);
    if (img_YUV.cols % MACROBLOCKSIZE != 0) {
        int patch = MACROBLOCKSIZE - img_YUV.cols % MACROBLOCKSIZE;
        img_temp1 = img_YUV(Range(0, img_YUV.rows), Range(img_YUV.cols - 1, img_YUV.cols));
        for (int i = 0; i < patch; i++) {
            img_temp2 = img_16base(Range(0, img_YUV.rows), Range(img_YUV.cols + i, img_YUV.cols + i + 1));
            img_temp1.copyTo(img_temp2);
        }
       
    }
    if (img_YUV.rows % MACROBLOCKSIZE != 0) {
        int patch = MACROBLOCKSIZE - img_YUV.rows % MACROBLOCKSIZE;
        img_temp1 = img_16base(Range(img_YUV.rows - 1, img_YUV.rows), Range(0, img_16base.cols));
        for (int i = 0; i < patch; i++) {
            img_temp2 = img_16base(Range(img_YUV.rows + i, img_YUV.rows + i + 1), Range(0, img_16base.cols));
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
            U.at<short>(i, j) = (img_16base.at<Vec3b>(i * 2, j * 2)[1] + img_16base.at<Vec3b>(i * 2 + 1, j * 2)[1] + img_16base.at<Vec3b>(i * 2, j * 2 + 1)[1] + img_16base.at<Vec3b>(i * 2 + 1, j * 2 + 1)[1]) / 4;
            V.at<short>(i, j) = (img_16base.at<Vec3b>(i * 2, j * 2)[2] + img_16base.at<Vec3b>(i * 2 + 1, j * 2)[2] + img_16base.at<Vec3b>(i * 2, j * 2 + 1)[2] + img_16base.at<Vec3b>(i * 2 + 1, j * 2 + 1)[2]) / 4;
            Y.at<short>(i * 2, j * 2) = img_16base.at<Vec3b>(i * 2, j * 2)[0];
            Y.at<short>(i * 2 + 1, j * 2) = img_16base.at<Vec3b>(i * 2 + 1, j * 2)[0];
            Y.at<short>(i * 2, j * 2 + 1) = img_16base.at<Vec3b>(i * 2, j * 2 + 1)[0];
            Y.at<short>(i * 2 + 1, j * 2 + 1) = img_16base.at<Vec3b>(i * 2 + 1, j * 2 + 1)[0];
        }
    }
    // Mat test;
    // Y.convertTo(test, CV_8U);
    // imshow("Y", test);
    // U.convertTo(test, CV_8U);
    // imshow("U", test);
    // V.convertTo(test, CV_8U);
    // imshow("V", test);
}

Mat WebP::predictiveCoding(int block_num, int channel) {
    Mat temp_mat[4], predict_mat;
    int distance = INF, block_size = MACROBLOCKSIZE, index = 0; 
    Mat * channel_mat;
    switch (channel) {
        case Y_CHANNEL:
            channel_mat = &Y;
            break;
        case U_CHANNEL:
            channel_mat = &U;
            block_size /= 2; 
            break;
        case V_CHANNEL:
            channel_mat = &V;
            block_size /= 2; 
            break;
        default:
            cout<<"[ERROR]: wrong channel!"<<endl;
            exit(-1);
            break;
    }
    temp_mat[0] = hPredict(block_num, channel, *channel_mat);
    temp_mat[1] = vPredict(block_num, channel, *channel_mat);
    temp_mat[2] = dcPredict(block_num, channel, *channel_mat);     
    temp_mat[3] = tmPredict(block_num, channel, *channel_mat); 

    for (int i = 0; i < 4; i++) {
        int temp = residualLength(temp_mat[i]);
        if (temp < distance) {
            distance = temp;
            index = i;
        }
    }
    // add forecast method type to vector 
    predict_type.push_back(index);
    return temp_mat[index];
}

Mat WebP::hPredict(int block_num, int block_size, Mat channel_mat) {
    Mat residual_mat, predict_mat, origin_mat;
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    predict_mat.zeros(block_size, block_size, CV_16S);
    origin_mat = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size)).clone();
    if (block_col != 0) {
        Mat temp = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size - 1, block_col * block_size + block_size - 1));
        temp.copyTo(predict_mat);
    } else {
        Mat temp0 = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size - 1));
        Mat temp1 = predict_mat(Range(0, block_size), Range(1, block_size));
        temp0.copyTo(temp1);
    }
    absdiff(origin_mat, predict_mat, residual_mat);
    return residual_mat;
}


Mat WebP::vPredict(int block_num, int block_size, Mat channel_mat) {
    Mat residual_mat, predict_mat, origin_mat;
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    predict_mat.zeros(block_size, block_size, CV_16S);
    origin_mat = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size)).clone();
    if (block_row != 0) {
        Mat temp = channel_mat(Range(block_row * block_size - 1, block_row * block_size + block_size - 1), Range(block_col * block_size, block_col * block_size + block_size));
        temp.copyTo(predict_mat);
    } else {
        Mat temp0 = channel_mat(Range(block_row * block_size, block_row * block_size + block_size - 1), Range(block_col * block_size, block_col * block_size + block_size));
        Mat temp1 = predict_mat(Range(1, block_size), Range(0, block_size));
        temp0.copyTo(temp1);
    }
    absdiff(origin_mat, predict_mat, residual_mat);
    return residual_mat;
}

Mat WebP::dcPredict(int block_num, int block_size, Mat channel_mat) {
    Mat residual_mat, predict_mat, origin_mat;
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    predict_mat.zeros(block_size, block_size, CV_16S);
    origin_mat = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size)).clone();
    if (block_col != 0) {
        Mat temp = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size - 1, block_col * block_size + block_size - 1));
        temp.copyTo(predict_mat);
    } else {
        Mat temp0 = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size - 1));
        Mat temp1 = predict_mat(Range(0, block_size), Range(1, block_size));
        temp0.copyTo(temp1);
    }
    absdiff(origin_mat, predict_mat, residual_mat);
    return residual_mat;
}

Mat WebP::tmPredict(int block_num, int block_size, Mat channel_mat) {
    Mat residual_mat, predict_mat, origin_mat;
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    predict_mat.zeros(block_size, block_size, CV_16S);
    origin_mat = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size)).clone();
    if (block_col != 0) {
        Mat temp = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size - 1, block_col * block_size + block_size - 1));
        temp.copyTo(predict_mat);
    } else {
        Mat temp0 = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size - 1));
        Mat temp1 = predict_mat(Range(0, block_size), Range(1, block_size));
        temp0.copyTo(temp1);
    }
    absdiff(origin_mat, predict_mat, residual_mat);
    return residual_mat;
}

Mat WebP::dct(int block_num, int channel) {
    int block_size = MACROBLOCKSIZE; 
    Mat * channel_mat;
    Mat residual_mat, result_mat;
    switch (channel) {
    case Y_CHANNEL:
        channel_mat = &Y;
        break;
    case U_CHANNEL:
        channel_mat = &U;
        block_size /= 2; 
        break;
    case V_CHANNEL:
        channel_mat = &V;
        block_size /= 2; 
        break;
    default:
        cout<<"[ERROR]: wrong channel!"<<endl;
        exit(-1);
        break;
    }
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    //temp_mat = (*channel_mat)(Range(block_row * block_size, (block_row + 1) * block_size), Range(block_col * block_size, (block_col + 1) * block_size));
    residual_mat = predictiveCoding(block_num, channel);
    cv::dct(residual_mat, result_mat);

    return result_mat;
}

// using E1 distance
int WebP::EuclideanDistance(Mat mat1, Mat mat2) {

}

int WebP::residualLength(Mat mat) {
    double minValue, maxValue;
    Point minLoc, maxLoc;
    minMaxLoc(mat, &minValue, &maxValue, &minLoc, &maxLoc);
    return (int)(abs(minValue) > maxValue ? abs(minValue) : abs(maxValue));
}