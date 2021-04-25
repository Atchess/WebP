#include "WebP.h"

WebP::WebP(String path) {
    this->path = path;
    initQuantizationTable();
    initDctMat();
    //test();
    compress();
    writeData();
    uncompress();
    imshow("test", img_reconstruct);
    imwrite("p1.bmp", img_reconstruct);
    Mat test = U_reconstruct;
    test.convertTo(test, CV_8U);
    imshow("dd", test);
    test = Y_reconstruct;
    test.convertTo(test, CV_8U);
    imshow("ddY", test);
    waitKey();
}

void WebP::initQuantizationTable() {
    //float quan_Y[16][16] = {7, 7, 7, 7, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    //                        , 7, 7, 7, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 17
    //                        , 7, 7, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 17, 18
    //                        , 7, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 17, 18, 20
    //                        , 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 17, 18, 20, 22
    //                        , 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 17, 18, 20, 22, 24
    //                        , 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 17, 18, 20, 22, 24, 26
    //                        , 1, 1, 1, 1, 1, 1, 1, 1, 1, 17, 18, 20, 22, 24, 26, 28
    //                        , 1, 1, 1, 1, 1, 1, 1, 1, 17, 18, 20, 22, 24, 26, 28, 30
    //                        , 1, 1, 1, 1, 1, 1, 1, 1, 18, 20, 22, 24, 26, 28, 30, 33
    //                        , 1, 1, 1, 1, 1, 1, 17, 18, 20, 22, 24, 26, 28, 30, 33, 36
    //                        , 1, 1, 1, 1, 1, 17, 18, 20, 22, 24, 26, 28, 30, 33, 36, 39
    //                        , 1, 1, 1, 1, 17, 18, 20, 22, 24, 26, 28, 30, 33, 36, 39, 42
    //                        , 1, 1, 1, 17, 18, 20, 2, 24, 26, 28, 30, 33, 36, 39, 42, 45
    //                        , 1, 1, 17, 18, 20, 22, 24, 26, 28, 30, 33, 36, 39, 42, 45, 49
    //                        , 1, 17, 18, 20, 22, 24, 26, 28, 30, 33, 36, 39, 42, 45, 49, 52};
    float quan_UV[8][8] = { 4, 1.5, 1, 1, 1, 1, 1, 1
                            , 1.5, 1, 1, 1, 1, 1, 1, 14
                            , 1, 1, 1, 1, 1, 1, 14, 15
                            , 1, 1, 1, 1, 1, 14, 15, 16
                            , 1, 1, 1, 1, 14, 15, 16, 18
                            , 1, 1, 1, 14, 15, 16, 18, 20
                            , 1, 1, 14, 15, 16, 18, 20, 22
                            , 1, 14, 15, 16, 18, 20, 22, 23
                            };
    float quan_Y[16][16] = { 8, 2, 1.5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
                            , 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 17
                            , 1.5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 17, 18
                            , 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 17, 18, 20
                            , 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 17, 18, 20, 22
                            , 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 17, 18, 20, 22, 24
                            , 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 17, 18, 20, 22, 24, 26
                            , 1, 1, 1, 1, 1, 1, 1, 1, 1, 17, 18, 20, 22, 24, 26, 28
                            , 1, 1, 1, 1, 1, 1, 1, 1, 17, 18, 20, 22, 24, 26, 28, 30
                            , 1, 1, 1, 1, 1, 1, 1, 1, 18, 20, 22, 24, 26, 28, 30, 33
                            , 1, 1, 1, 1, 1, 1, 17, 18, 20, 22, 24, 26, 28, 30, 33, 36
                            , 1, 1, 1, 1, 1, 17, 18, 20, 22, 24, 26, 28, 30, 33, 36, 39
                            , 1, 1, 1, 1, 17, 18, 20, 22, 24, 26, 28, 30, 33, 36, 39, 42
                            , 1, 1, 1, 17, 18, 20, 2, 24, 26, 28, 30, 33, 36, 39, 42, 45
                            , 1, 1, 17, 18, 20, 22, 24, 26, 28, 30, 33, 36, 39, 42, 45, 49
                            , 1, 17, 18, 20, 22, 24, 26, 28, 30, 33, 36, 39, 42, 45, 49, 52};
    //float quan_UV[8][8] = { 8, 8, 8, 9, 1, 1, 1, 1
    //                        , 8, 8, 9, 1, 1, 1, 1, 14
    //                        , 8, 9, 1, 1, 1, 1, 14, 15
    //                        , 9, 1, 1, 1, 1, 14, 15, 16
    //                        , 1, 1, 1, 1, 14, 15, 16, 18
    //                        , 1, 1, 1, 14, 15, 16, 18, 20
    //                        , 1, 1, 14, 15, 16, 18, 20, 22
    //                        , 1, 14, 15, 16, 18, 20, 22, 23
    //                        };
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (quan_UV[i][j] == 1) {
                quan_UV[i][j] = 0.1;
            } else {
                quan_UV[i][j] = 1;
            }
        }
    }
//
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (quan_Y[i][j] == 1) {
                quan_Y[i][j] = 0.1;
            } else {
                quan_Y[i][j] = 1;
            }
        }
    }
    quantizationTable_Y = Mat(MACROBLOCKSIZE, MACROBLOCKSIZE, CV_32F, quan_Y).clone();
    quantizationTable_UV = Mat(MACROBLOCKSIZE / 2, MACROBLOCKSIZE / 2, CV_32F, quan_UV).clone();
    //quantizationTable_Y = Mat::ones(MACROBLOCKSIZE, MACROBLOCKSIZE, CV_32F) / 10.0;
    //quantizationTable_UV = Mat::ones(MACROBLOCKSIZE / 2, MACROBLOCKSIZE / 2, CV_32F) / 10.0;
    //quantizationTable_Y.at<float>(0, 0) = 8;
    //quantizationTable_Y.at<float>(0, 1) = 4;
    //quantizationTable_Y.at<float>(1, 0) = 4;
    //quantizationTable_Y.at<float>(1, 1) = 2;
    //quantizationTable_UV.at<float>(0, 0) = 4;
}

void WebP::test() {
    imagePretreatment(path);
    predict_type.clear();
    subSampling();
    int block_num;
    Mat temp0, temp1, temp2;
    Mat y_pre, u_pre, v_pre;
    y_pre = Y.clone();
    u_pre = U.clone();
    v_pre = V.clone();

    Y_reconstruct.create(Y.rows, Y.cols, CV_16S);
    U_reconstruct.create(U.rows, U.cols, CV_16S);
    V_reconstruct.create(V.rows, V.cols, CV_16S);

    for (int i = 0; i < block_rows; i ++) {
        for (int j = 0; j < block_cols; j ++) {
            block_num = i * block_cols + j;
            temp0 = predictiveCoding(block_num, Y_CHANNEL);
            temp1 = predictiveCoding(block_num, U_CHANNEL);
            temp2 = predictiveCoding(block_num, V_CHANNEL);

            temp0.convertTo(temp0, CV_32F);
            temp1.convertTo(temp1, CV_32F);
            temp2.convertTo(temp2, CV_32F);

            cv::dct(temp0, temp0);
            cv::dct(temp1, temp1);
            cv::dct(temp2, temp2);

            //cout<<temp0<<endl;

            //temp0.convertTo(temp0, CV_16S);
            //temp1.convertTo(temp1, CV_16S);
            //temp2.convertTo(temp2, CV_16S);

            temp0 = zigzag(temp0);
            temp1 = zigzag(temp1);
            temp2 = zigzag(temp2);
//
            temp0 = inzigzag(temp0);
            temp1 = inzigzag(temp1);
            temp2 = inzigzag(temp2);


            temp0.convertTo(temp0, CV_32F);
            temp1.convertTo(temp1, CV_32F);
            temp2.convertTo(temp2, CV_32F);

            //cout<<temp0<<endl;

            cv::idct(temp0, temp0);
            cv::idct(temp1, temp1);            
            cv::idct(temp2, temp2);

            temp0.convertTo(temp0, CV_16S);
            temp1.convertTo(temp1, CV_16S);
            temp2.convertTo(temp2, CV_16S);

            temp0.copyTo(y_pre(Range(i * MACROBLOCKSIZE, (i + 1) * MACROBLOCKSIZE), Range(j * MACROBLOCKSIZE, (j + 1) * MACROBLOCKSIZE)));
            temp1.copyTo(u_pre(Range(i * MACROBLOCKSIZE / 2, (i + 1) * MACROBLOCKSIZE / 2), Range(j * MACROBLOCKSIZE / 2, (j + 1) * MACROBLOCKSIZE / 2)));
            temp2.copyTo(v_pre(Range(i * MACROBLOCKSIZE / 2, (i + 1) * MACROBLOCKSIZE / 2), Range(j * MACROBLOCKSIZE / 2, (j + 1) * MACROBLOCKSIZE / 2)));

        }
    }
    reconstruct_type.assign(predict_type.begin(), predict_type.end());
    for (int i = 0; i < block_rows; i ++) {
        for (int j = 0; j < block_cols; j ++) {
            block_num = i * block_cols + j;
            try
            {
            temp0 = inpredictCoding(block_num, MACROBLOCKSIZE, Y_CHANNEL, y_pre(Range(i * MACROBLOCKSIZE, (i + 1) * MACROBLOCKSIZE), Range(j * MACROBLOCKSIZE, (j + 1) * MACROBLOCKSIZE)));
            temp1 = inpredictCoding(block_num, MACROBLOCKSIZE / 2, U_CHANNEL, u_pre(Range(i * MACROBLOCKSIZE / 2, (i + 1) * MACROBLOCKSIZE / 2), Range(j * MACROBLOCKSIZE / 2, (j + 1) * MACROBLOCKSIZE / 2)));
            temp2 = inpredictCoding(block_num, MACROBLOCKSIZE / 2, V_CHANNEL, v_pre(Range(i * MACROBLOCKSIZE / 2, (i + 1) * MACROBLOCKSIZE / 2), Range(j * MACROBLOCKSIZE / 2, (j + 1) * MACROBLOCKSIZE / 2)));
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            
            temp0.copyTo(Y_reconstruct(Range(i * MACROBLOCKSIZE, (i + 1) * MACROBLOCKSIZE), Range(j * MACROBLOCKSIZE, (j + 1) * MACROBLOCKSIZE)));
            temp1.copyTo(U_reconstruct(Range(i * MACROBLOCKSIZE / 2, (i + 1) * MACROBLOCKSIZE / 2), Range(j * MACROBLOCKSIZE / 2, (j + 1) * MACROBLOCKSIZE / 2)));
            temp2.copyTo(V_reconstruct(Range(i * MACROBLOCKSIZE / 2, (i + 1) * MACROBLOCKSIZE / 2), Range(j * MACROBLOCKSIZE / 2, (j + 1) * MACROBLOCKSIZE / 2)));

        }
    }
    Mat YUVn;
    YUVn.create(Y.rows, Y.cols, CV_16SC3);
    for (int i = 0; i < YUVn.rows; i++) {
        for (int j = 0; j < YUVn.cols; j++) {
            YUVn.at<Vec3s>(i, j)[0] = Y_reconstruct.at<short>(i, j);
            YUVn.at<Vec3s>(i, j)[1] = U_reconstruct.at<short>(i / 2, j / 2);
            YUVn.at<Vec3s>(i, j)[2] = V_reconstruct.at<short>(i / 2, j / 2);
        }
    }
    YUVn.convertTo(temp0, CV_8UC3);
    Mat test;
    cvtColor(temp0, test, COLOR_YUV2BGR);
    imshow("drrd", test);
    Y_reconstruct.convertTo(temp0, CV_8U);
    imshow("dd", temp0);
    V_reconstruct.convertTo(temp0, CV_8U);
    imshow("ddf", temp0);
    U_reconstruct.convertTo(temp0, CV_8U);
    imshow("drd", temp0);
    waitKey();
}

WebP::~WebP() {

}

void WebP::compress() {
    imagePretreatment(path);
    subSampling();
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
    int hist[4] = {0};
    for (int i = 0; i < predict_type.size(); i++) {
        hist[predict_type.at(i)] ++;
    }
    
    DPCMAndRimCoding();
    //arithmeticCoding();
}

// the function is so bad, but i don't want to rewrite it. 
// maybe when i finish the project, i will consider rewriting it.
Mat WebP::zigzag(Mat input) {
    Mat zigMat = Mat::zeros(1, input.cols * input.rows, CV_16S);
    input.convertTo(input, CV_16S);
    for (int i = 0, k = 0, x = 0, y = 0; i < input.rows; i++) {
        for (int j = 0; j < input.cols; j++) {
            zigMat.at<short>(0, x * input.cols + y) = input.at<short>(i, j);
            if ((x == input.rows - 1 || x == 0) && y % 2 == 0) {
                y ++;
                continue;
            }
            if ((y == input.cols - 1 || y == 0) && x % 2 == 1) {
                x ++;
                continue;
            }
            if ((x + y) % 2 == 0) {
                x --;
                y ++;
            } else {
                x ++;
                y--;
            }
        }
    }
    return zigMat;
}

void WebP::imagePretreatment(String path) {
    img_origin = imread(path);
    Mat img_YUV, img_temp0, img_temp1, img_temp2;
    int cols, rows;
    cvtColor(img_origin, img_YUV, COLOR_RGB2YCrCb);
    cols = img_origin.cols % MACROBLOCKSIZE == 0 ? img_origin.cols : (img_origin.cols / MACROBLOCKSIZE + 1) * MACROBLOCKSIZE;
    rows = img_origin.rows % MACROBLOCKSIZE == 0 ? img_origin.rows : (img_origin.rows / MACROBLOCKSIZE + 1) * MACROBLOCKSIZE;
    height = img_origin.rows;
    width = img_origin.cols;
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
}

Mat WebP::predictiveCoding(int block_num, int channel) {
    Mat temp_mat[4], predict_mat;
    int distance = INF, block_size = MACROBLOCKSIZE, index = 0; 
    Mat channel_mat;
    switch (channel) {
        case Y_CHANNEL:
            channel_mat = Y;
            break;
        case U_CHANNEL:
            channel_mat = U;
            block_size /= 2; 
            break;
        case V_CHANNEL:
            channel_mat = V;
            block_size /= 2; 
            break;
        default:
            cout<<"[ERROR]: wrong channel!"<<endl;
            exit(-1);
            break;
    }

    temp_mat[0] = hPredict(block_num, block_size, channel_mat);
    temp_mat[1] = vPredict(block_num, block_size, channel_mat);
    temp_mat[2] = dcPredict(block_num, block_size, channel_mat);     
    temp_mat[3] = tmPredict(block_num, block_size, channel_mat); 
    
    for (int i = 0; i < 4; i++) {
        int temp = residualLength(temp_mat[i]);
        if (temp < distance) {
            distance = temp;
            index = i;
        }
    }
    // add forecast method type to vector 
    predict_type.push_back(3);
    return temp_mat[3];
}

Mat WebP::hPredict(int block_num, int block_size, Mat channel_mat) {
    Mat residual_mat, predict_mat, origin_mat;
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    predict_mat = Mat::zeros(block_size, block_size, CV_16S);
    origin_mat = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size)).clone();
    if (block_col != 0) {
        Mat temp = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size - 1, block_col * block_size + block_size - 1));
        temp.copyTo(predict_mat);
    } else {
        Mat temp0 = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size - 1));
        Mat temp1 = predict_mat(Range(0, block_size), Range(1, block_size));
        temp0.copyTo(temp1);
    }
    subtract(origin_mat, predict_mat, residual_mat);
    //cout<<origin_mat<<endl;
    //cout<<predict_mat<<endl;
    //cout<<residual_mat<<endl;
    return residual_mat;
}


Mat WebP::vPredict(int block_num, int block_size, Mat channel_mat) {
    Mat residual_mat, predict_mat, origin_mat;
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    predict_mat = Mat::zeros(block_size, block_size, CV_16S);
    origin_mat = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size)).clone();
    if (block_row != 0) {
        Mat temp = channel_mat(Range(block_row * block_size - 1, block_row * block_size + block_size - 1), Range(block_col * block_size, block_col * block_size + block_size));
        temp.copyTo(predict_mat);
    } else {
        Mat temp0 = channel_mat(Range(block_row * block_size, block_row * block_size + block_size - 1), Range(block_col * block_size, block_col * block_size + block_size));
        Mat temp1 = predict_mat(Range(1, block_size), Range(0, block_size));
        temp0.copyTo(temp1);
    }
    subtract(origin_mat, predict_mat, residual_mat);
    return residual_mat;
}

Mat WebP::dcPredict(int block_num, int block_size, Mat channel_mat) {
    Mat residual_mat, predict_mat, origin_mat;
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    int mean0, mean1;
    predict_mat = Mat::ones(block_size, block_size, CV_16S);
    origin_mat = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size)).clone();

    if (block_row != 0) {
        Mat temp = channel_mat(Range(block_row * block_size - 1, block_row * block_size), Range(block_col * block_size , block_col * block_size + block_size));
        mean0 = mean(temp).val[0];
    } else {
        mean0 = 0;
    }
    if (block_col != 0) {
        Mat temp = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size - 1, block_col * block_size));
        mean1 = mean(temp).val[0];
    } else {
        mean1 = 0;
    }

    predict_mat = predict_mat * ((mean0 + mean1) / 2);

    subtract(origin_mat, predict_mat, residual_mat);
    return residual_mat;
}

Mat WebP::tmPredict(int block_num, int block_size, Mat channel_mat) {
    Mat residual_mat, predict_mat, origin_mat, left_col_mat, up_row_mat;
    Mat temp_mat0, temp_mat1, temp_mat2; 
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    int leftup = 0;
    origin_mat = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size)).clone();
    if (block_row != 0) {
        up_row_mat = channel_mat(Range(block_row * block_size - 1, block_row * block_size), Range(block_col * block_size, block_col * block_size + block_size)).clone();
    } else {
        up_row_mat = Mat::zeros(1, block_size, CV_16S);
    }
    if (block_col != 0) {
        left_col_mat = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size - 1, block_col * block_size)).clone();
    } else {
        left_col_mat = Mat::zeros(block_size, 1, CV_16S);
    }
    if (block_row != 0 && block_col != 0) {
        leftup = channel_mat.at<short>(block_row * block_size - 1, block_col * block_size - 1);
    }

    temp_mat0 = Mat::ones(block_size, block_size, CV_16S);
    temp_mat1 = Mat::ones(block_size, block_size, CV_16S);
    temp_mat2 = Mat::ones(block_size, block_size, CV_16S) * leftup;
    for (int i = 0; i < block_size; i++) {
        left_col_mat.copyTo(temp_mat0(Range(0, block_size), Range(i, i + 1)));
        up_row_mat.copyTo(temp_mat1(Range(i, i + 1), Range(0, block_size)));
    }
    predict_mat = temp_mat0 + temp_mat1 - temp_mat2;
    subtract(origin_mat, predict_mat, residual_mat);
    return residual_mat;
}

Mat WebP::dct(int block_num, int channel) {
    Mat residual_mat, result_mat;
    residual_mat = predictiveCoding(block_num, channel);
    result_mat = mydct(residual_mat);
    //cout<<residual_mat<<endl;
    //cout<<result_mat<<endl;
    //result_mat.convertTo(result_mat, CV_16S);
    result_mat = result_mat / (channel == Y_CHANNEL ? quantizationTable_Y : quantizationTable_UV);
    return result_mat;
}
int WebP::residualLength(Mat mat) {
    double minValue, maxValue;
    Point minLoc, maxLoc;
    minMaxLoc(mat, &minValue, &maxValue, &minLoc, &maxLoc);
    return (int)(abs(minValue) > maxValue ? abs(minValue) : abs(maxValue));
}

Mat WebP::inzigzag(Mat input) {
    if (input.rows != 1) {
        cout<<"[ERROR]: inverse zigzag input wrong, the input mat is not one-dimensional"<<endl;
    }
    Mat result = Mat::zeros(sqrt(input.cols), sqrt(input.cols), CV_16S);
    input.convertTo(input, CV_16S);    
    for (int i = 0, k = 0, x = 0, y = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            result.at<short>(i, j) = input.at<short>(0, x * result.cols + y);
            if ((x == result.rows - 1 || x == 0) && y % 2 == 0) {
                y ++;
                continue;
            }
            if ((y == result.cols - 1 || y == 0) && x % 2 == 1) {
                x ++;
                continue;
            }
            if ((x + y) % 2 == 0) {
                x --;
                y ++;
            } else {
                x ++;
                y--;
            }
        }
    }
    return result;
}
Mat WebP::indct(int block_num, int channel) {
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    int block_size = MACROBLOCKSIZE;
    Mat channel_mat, zigzag_mat, residual_mat, reconstruct_mat, reconstruct_block_mat;
    switch (channel) {
        case Y_CHANNEL:
            channel_mat = idct_Y;
            reconstruct_mat = Y_reconstruct;
            break;
        case U_CHANNEL:
            channel_mat = idct_U;
            reconstruct_mat = U_reconstruct;
            block_size /= 2; 
            break;
        case V_CHANNEL:
            channel_mat = idct_V;
            reconstruct_mat = V_reconstruct;
            block_size /= 2; 
            break;
        default:
            cout<<"[ERROR]: wrong channel!"<<endl;
            exit(-1);
            break;
    }
    zigzag_mat = channel_mat(Range(block_num, block_num + 1), Range(0, block_size * block_size)).clone();
    Mat indct_mat = inzigzag(zigzag_mat);
    indct_mat.convertTo(indct_mat, CV_32F);
    indct_mat = indct_mat.mul(channel == Y_CHANNEL ? quantizationTable_Y : quantizationTable_UV);
    residual_mat = myidct(indct_mat);
    reconstruct_block_mat = residual_mat;
    //reconstruct_block_mat.convertTo(reconstruct_block_mat, CV_16S);
    reconstruct_block_mat = inpredictCoding(block_num, block_size, channel, residual_mat);    
    Mat temp_mat = reconstruct_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size));
    reconstruct_block_mat.copyTo(temp_mat);
    return reconstruct_block_mat;
}
Mat WebP::inpredictCoding(int block_num, int block_size, int channel, Mat residual_mat) {
    int type = reconstruct_type.at(block_num * 3 + channel);
    Mat reconstruct_mat, channel_mat;
    switch (channel) {
        case Y_CHANNEL:
            channel_mat = Y_reconstruct;
            break;
        case U_CHANNEL:
            channel_mat = U_reconstruct;
            break;
        case V_CHANNEL:
            channel_mat = V_reconstruct;
            break;
        default:
            cout<<"[ERROR]: wrong channel!"<<endl;
            exit(-1);
            break;
    }
    switch (type) {
        case H_PRED :
            try {
            reconstruct_mat = inhPredict(block_num, block_size, channel_mat, residual_mat);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            break;
        case V_PRED :
            reconstruct_mat = invPredict(block_num, block_size, channel_mat, residual_mat);
            break;
        case DC_PRED :
            reconstruct_mat = indcPredict(block_num, block_size, channel_mat, residual_mat);
            break;
        case TM_PRED :
            reconstruct_mat = intmPredict(block_num, block_size, channel_mat, residual_mat);
            break;
        default :
            cout<<"[ERROR]: Wrong reconstruction type!"<<endl;
    }
    return reconstruct_mat;
}
Mat WebP::reconstructImage() { 
    Mat YUV_reconstruct;
    int cols, rows;
    cols = width % MACROBLOCKSIZE == 0 ? width : (width / MACROBLOCKSIZE + 1) * MACROBLOCKSIZE;
    rows = height % MACROBLOCKSIZE == 0 ? height : (height / MACROBLOCKSIZE + 1) * MACROBLOCKSIZE;
    Y_reconstruct.create(rows, cols, CV_16S);
    U_reconstruct.create(rows / 2, cols / 2, CV_16S);
    V_reconstruct.create(rows / 2, cols / 2, CV_16S);
    YUV_reconstruct.create(rows, cols, CV_16SC3);

    int block_num;
    for (int i = 0; i < block_rows; i ++) {
        for (int j = 0; j < block_cols; j ++) {
            block_num = i * block_cols + j;
            indct(block_num, Y_CHANNEL);
            indct(block_num, U_CHANNEL);
            indct(block_num, V_CHANNEL);
        }
    }

    for (int i = 0; i < YUV_reconstruct.rows; i++) {
        for (int j = 0; j < YUV_reconstruct.cols; j++) {
            YUV_reconstruct.at<Vec3s>(i, j)[0] = Y_reconstruct.at<short>(i, j);
            YUV_reconstruct.at<Vec3s>(i, j)[1] = U_reconstruct.at<short>(i / 2, j / 2);
            YUV_reconstruct.at<Vec3s>(i, j)[2] = V_reconstruct.at<short>(i / 2, j / 2);
        }
    }
    YUV_reconstruct.convertTo(YUV_reconstruct, CV_8UC3);
    cvtColor(YUV_reconstruct, img_reconstruct_16base, COLOR_YCrCb2RGB);
    img_reconstruct = Mat::zeros(height, width, CV_16S);
    (img_reconstruct_16base(Range(0, height), Range(0, width))).copyTo(img_reconstruct);
    return YUV_reconstruct;
}
void WebP::uncompress() {

    idct_Y.create( block_cols * block_rows, MACROBLOCKSIZE * MACROBLOCKSIZE, CV_16S);
    idct_U.create( block_cols * block_rows, MACROBLOCKSIZE / 2 * MACROBLOCKSIZE / 2, CV_16S);
    idct_V.create( block_cols * block_rows, MACROBLOCKSIZE / 2 * MACROBLOCKSIZE / 2, CV_16S);
    //reconstruct_type.assign(predict_type.begin(), predict_type.end());
    //idct_Y = dct_Y.clone();
    //idct_U = dct_U.clone();
    //idct_V = dct_V.clone();
    //ivec_DAR_Y.assign(vec_DAR_Y.begin(), vec_DAR_Y.end());
    //ivec_DAR_U.assign(vec_DAR_U.begin(), vec_DAR_U.end());
    //ivec_DAR_V.assign(vec_DAR_V.begin(), vec_DAR_V.end());
    //deArithmeticCoding();
    readData();
    deDPCMAndRimCoding();
    
    reconstructImage();

}

Mat WebP::inhPredict(int block_num, int block_size, Mat channel_mat, Mat residual_mat) {
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    Mat previous_col, residual_col, origin_col, origin_mat;
    residual_mat.convertTo(residual_mat, CV_16S);
    origin_mat = Mat::zeros(block_size, block_size, CV_16S);
    if (block_col != 0) {
        previous_col = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size - 1, block_col * block_size)).clone();
        for (int i = 0; i < block_size; i++) {
            residual_col = residual_mat(Range(0, block_size), Range(i, i + 1));
            origin_col = residual_col + previous_col;
            origin_col.copyTo(origin_mat(Range(0, block_size), Range(i, i + 1)));
            previous_col = origin_col.clone();
        }
    } else {
        previous_col = Mat::zeros(block_size, 1, CV_16S);
        for (int i = 0; i < block_size; i++) {
            residual_col = residual_mat(Range(0, block_size), Range(i, i + 1));
            origin_col = residual_col + previous_col;
            origin_col.copyTo(origin_mat(Range(0, block_size), Range(i, i + 1)));
            previous_col = origin_col.clone();
        }
    }
    return origin_mat;
}
Mat WebP::invPredict(int block_num, int block_size, Mat channel_mat, Mat residual_mat) {
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    Mat previous_row, residual_row, origin_row, origin_mat;
    residual_mat.convertTo(residual_mat, CV_16S);
    origin_mat = Mat::zeros(block_size, block_size, CV_16S);
    if (block_row != 0) {
        previous_row = channel_mat(Range(block_row * block_size - 1, block_row * block_size), Range(block_col * block_size, block_col * block_size + block_size));
        for (int i = 0; i < block_size; i++) {
            residual_row = residual_mat(Range(i, i + 1), Range(0, block_size));
            origin_row = residual_row + previous_row;
            origin_row.copyTo(origin_mat(Range(i, i + 1), Range(0, block_size)));
            previous_row = origin_row.clone();
        }
    } else {
        previous_row = Mat::zeros(1, block_size, CV_16S);
        for (int i = 0; i < block_size; i++) {
            residual_row = residual_mat(Range(i, i + 1), Range(0, block_size));
            origin_row = residual_row + previous_row;
            origin_row.copyTo(origin_mat(Range(i, i + 1), Range(0, block_size)));
            previous_row = origin_row.clone();
        }
    }
    //cout<<origin_mat<<endl;
    return origin_mat;
}
Mat WebP::indcPredict(int block_num, int block_size, Mat channel_mat, Mat residual_mat) {
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    Mat predictive_mat, origin_mat;
    residual_mat.convertTo(residual_mat, CV_16S);
    origin_mat = Mat::zeros(block_size, block_size, CV_16S);
    predictive_mat = Mat::ones(block_size, block_size, CV_16S);
    int mean0, mean1;
    if (block_row != 0) {
        Mat temp = channel_mat(Range(block_row * block_size - 1, block_row * block_size), Range(block_col * block_size , block_col * block_size + block_size));
        mean0 = mean(temp).val[0];
    } else {
        mean0 = 0;
    }
    if (block_col != 0) {
        Mat temp = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size - 1, block_col * block_size));
        mean1 = mean(temp).val[0];
    } else {
        mean1 = 0;
    }
    predictive_mat = predictive_mat * ((mean0 + mean1) / 2);
    origin_mat = predictive_mat + residual_mat;
    return origin_mat;
}
Mat WebP::intmPredict(int block_num, int block_size, Mat channel_mat, Mat residual_mat) {
    Mat predict_mat, origin_mat, left_col_mat, up_row_mat;
    Mat temp_mat0, temp_mat1, temp_mat2; 
    int block_row = block_num / block_cols;
    int block_col = block_num % block_cols;
    int leftup = 0;
    residual_mat.convertTo(residual_mat, CV_16S);
    if (block_row != 0) {
        up_row_mat = channel_mat(Range(block_row * block_size - 1, block_row * block_size), Range(block_col * block_size, block_col * block_size + block_size)).clone();
    } else {
        up_row_mat = Mat::zeros(1, block_size, CV_16S);
    }
    if (block_col != 0) {
        left_col_mat = channel_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size - 1, block_col * block_size)).clone();
    } else {
        left_col_mat = Mat::zeros(block_size, 1, CV_16S);
    }
    if (block_row != 0 && block_col != 0) {
        leftup = channel_mat.at<short>(block_row * block_size - 1, block_col * block_size - 1);
    }

    temp_mat0 = Mat::ones(block_size, block_size, CV_16S);
    temp_mat1 = Mat::ones(block_size, block_size, CV_16S);
    temp_mat2 = Mat::ones(block_size, block_size, CV_16S) * leftup;
    for (int i = 0; i < block_size; i++) {
        left_col_mat.copyTo(temp_mat0(Range(0, block_size), Range(i, i + 1)));
        up_row_mat.copyTo(temp_mat1(Range(i, i + 1), Range(0, block_size)));
    }
    predict_mat = temp_mat0 + temp_mat1 - temp_mat2;
    origin_mat = predict_mat + residual_mat;
    return origin_mat;
}


vector<short> WebP::DPCMAndRim(Mat mat) {
    vector<short> res;    //result of dpcm and rim-length

    res.push_back(mat.at<short>(0, 0));
    for (int i = 1; i < mat.rows; i++) {
        res.push_back(mat.at<short>(i, 0) - mat.at<short>(i - 1, 0));
    }

    for (int i = 0; i < mat.rows; i++) {
        int zeroCounter = 0;
        for (int j = 1; j < mat.cols; j++) {
            if (mat.at<short>(i, j) == 0 && j != mat.cols - 1) {
                zeroCounter++;
            }
            else {
                res.push_back(zeroCounter);
                res.push_back(mat.at<short>(i, j));
                zeroCounter = 0;
            }
        }
    }
    return res;
}

void WebP::DPCMAndRimCoding() {
    vec_DAR_Y = DPCMAndRim(dct_Y);
    vec_DAR_U = DPCMAndRim(dct_U);
    vec_DAR_V = DPCMAndRim(dct_V);
}

Mat WebP::deDPCMAndRim(vector<short> vec, int channel) {

    int DCNumber = block_rows * block_cols;
    int col = 0;
    Mat resMat;
    switch (channel)
    {
    case Y_CHANNEL:
        col = MACROBLOCKSIZE * MACROBLOCKSIZE;
        break;
    case U_CHANNEL:
        col = MACROBLOCKSIZE / 2 * MACROBLOCKSIZE / 2;
        break;
    case V_CHANNEL:
        col = MACROBLOCKSIZE / 2 * MACROBLOCKSIZE / 2;
        break;
    default:
        break;
    }
    resMat.create(DCNumber, col, CV_16S);

    resMat.at<short>(0, 0) = vec[0];
    for (int i = 1; i < DCNumber; i++) {
        resMat.at<short>(i, 0) = resMat.at<short>(i - 1, 0) + vec[i];
    }

    int zeroFlag = 1;
    int rowCounter = 0;
    int colCounter = 1;
    for (int i = DCNumber; i < vec.size(); i++) {
        if (zeroFlag) {
            for (int j = 0; j < vec[i]; j++) {
                resMat.at<short>(rowCounter, colCounter) = 0;
                colCounter++;
            }
            zeroFlag = 0;
        }
        else {
            resMat.at<short>(rowCounter, colCounter) = vec[i];
            colCounter++;
            zeroFlag = 1;
            if (colCounter == col) {
                colCounter = 1;
                rowCounter++;
            }
        }
    }
    return resMat;
}

void WebP::deDPCMAndRimCoding() {
    
    idct_Y = deDPCMAndRim(ivec_DAR_Y, Y_CHANNEL);
    idct_U = deDPCMAndRim(ivec_DAR_U, U_CHANNEL);
    idct_V = deDPCMAndRim(ivec_DAR_V, V_CHANNEL);
}

double WebP::arithmetic(vector<short> vec, map<short, MyRange> * arithmeticMap, int * lenBeforeArith) {
    
    map<short, int> frequencyMap;
    for (vector<short>::iterator it = vec.begin(); it != vec.end(); it++) {
        frequencyMap.insert(pair<short, int>(*it, 0));
        frequencyMap[*it]++;
    }
    
    int totalFrequency = 0;
    for (map<short, int>::iterator it = frequencyMap.begin(); it != frequencyMap.end(); it++) {
        totalFrequency += it->second;
    }

    double low = 0.0, high = 0.0;
    (*arithmeticMap).clear();
    for (map<short, int>::iterator it = frequencyMap.begin(); it != frequencyMap.end(); it++) {
        high = low + it->second * 1.0 / totalFrequency;
        MyRange range;
        range.start = low;
        range.end = high;
        (*arithmeticMap).insert(pair<short, MyRange>(it->first, range));
        low = high;
    }

    /*for (map<short, MyRange>::iterator it = arithmeticMap.begin(); it != arithmeticMap.end(); it++) {
        cout << it->first << "-" << it->second.start << "," << it->second.end << endl;
    }*/

    (*lenBeforeArith) = 0;
    double lowRange = 0.0, highRange = 1.0;
    for (vector<short>::iterator it = vec.begin(); it != vec.end(); it++) {
        double delta = highRange - lowRange;
        highRange = lowRange + delta * (*arithmeticMap)[*it].end;
        lowRange = lowRange + delta * (*arithmeticMap)[*it].start;
        (*lenBeforeArith)++;
    }

    return lowRange;
}

void WebP::arithmeticCoding() {
    res_Y = arithmetic(vec_DAR_Y, &(arithmeticMap_Y), &(lenBeforeArith_Y));
    res_U = arithmetic(vec_DAR_U, &(arithmeticMap_U), &(lenBeforeArith_U));
    res_V = arithmetic(vec_DAR_V, &(arithmeticMap_V), &(lenBeforeArith_V));
}

vector<short> WebP::deArithmetic(double code, int channel) {
    double lowInterval;
    double highInterval;
    vector<short> resVec;
    map<short, MyRange> arithmeticMap;
    int lenBeforeArith;

    switch (channel)
    {
    case Y_CHANNEL:
        arithmeticMap = arithmeticMap_Y;
        lenBeforeArith = lenBeforeArith_Y;
        break;
    case U_CHANNEL:
        arithmeticMap = arithmeticMap_U;
        lenBeforeArith = lenBeforeArith_U;
        break;
    case V_CHANNEL:
        arithmeticMap = arithmeticMap_V;
        lenBeforeArith = lenBeforeArith_V;
        break;
    default:
        break;
    }
    for (map<short, MyRange>::iterator it = arithmeticMap.begin(); it != arithmeticMap.end(); it++) {
        if ((it->second).start <= code && (it->second).end>code) {
            lowInterval = (it->second).start;
            highInterval = (it->second).end;
            resVec.push_back(it->first);
            break;
        }
    }

    for (int i = 0; i < lenBeforeArith - 1; i++) {
        double deltaInterval;
        deltaInterval = highInterval - lowInterval;
        code -= lowInterval;
        code /= deltaInterval;

        for (map<short, MyRange>::iterator it = arithmeticMap.begin(); it != arithmeticMap.end(); it++) {
            if ((it->second).start <= code && (it->second).end>code) {
                lowInterval = (it->second).start;
                highInterval = (it->second).end;
                resVec.push_back(it->first);
                break;
            }
        }
    }
    return resVec;
}

void WebP::deArithmeticCoding() {

    ivec_DAR_Y = deArithmetic(res_Y, Y_CHANNEL);
    ivec_DAR_U = deArithmetic(res_U, U_CHANNEL);
    ivec_DAR_V = deArithmetic(res_V, V_CHANNEL);

}

void WebP::writeData() {
    filename = "test.bin";
    outfile.open(filename, ios::binary);
    unsigned short h = height;
    unsigned short w = width;
    outfile.write((char *)&h, sizeof(short));
    outfile.write((char *)&w, sizeof(short));
    writeTypeData();
    writeRunLengthCode();
    outfile.close();
    //readData();
    //for (int i = 0; i < predict_type.size(); i++) {
    //    if (predict_type.at(i) != reconstruct_type.at(i)) {
    //        cout <<"wrong"<<endl;
    //    }
    //}
    //for (int i = 0; i < vec_DAR_Y.size(); i++) {
    //    if (vec_DAR_Y.at(i) != ivec_DAR_Y.at(i)) {
    //        cout<<i<<endl;
    //        cout<<vec_DAR_Y.at(i)<<" "<<ivec_DAR_Y.at(i)<<endl;
    //    }
    //}
    //for (int i = 0; i < vec_DAR_U.size(); i++) {
    //    if (vec_DAR_U.at(i) != ivec_DAR_U.at(i)) {
    //        cout<<i<<endl;
    //    }
    //}
    //for (int i = 0; i < vec_DAR_V.size(); i++) {
    //    if (vec_DAR_V.at(i) != ivec_DAR_V.at(i)) {
    //        cout<<i<<endl;
    //    }
    //}
    //cout<<"end"<<endl;
    
    
}

void WebP::writeTypeData() {
    char buffer = 0;
    int type, i;
    for (i = 0; i < predict_type.size(); i++) {
        type = predict_type.at(i);
        buffer = buffer | (type << (( i % 4 ) * 2));
        if (i % 4 == 3) {
            outfile.write(&buffer, sizeof(char));
            buffer = 0;
        }
    }
    outfile.write(&buffer, sizeof(char));
}


void WebP::writeArithmeticCode() {

}

void WebP::writeRunLengthCode() {
    int block_num = block_cols * block_rows;
    short num_s;
    unsigned char num_uc;

    lenBeforeArith_Y = vec_DAR_Y.size();
    lenBeforeArith_U = vec_DAR_U.size();
    lenBeforeArith_V = vec_DAR_V.size();

    outfile.write((char *)&lenBeforeArith_Y, sizeof(int));
    outfile.write((char *)&lenBeforeArith_U, sizeof(int));
    outfile.write((char *)&lenBeforeArith_V, sizeof(int));

    for (int i = 0; i < lenBeforeArith_Y; i++) {
        if (i < block_num) {
            num_s = (short)vec_DAR_Y.at(i);
            outfile.write((char *)&num_s, sizeof(short));
        } else {
            num_uc = (unsigned char)vec_DAR_Y.at(i);
            outfile.write((char *)&num_uc, sizeof(unsigned char));
            num_s = (short)vec_DAR_Y.at(++i);
            outfile.write((char *)&num_s, sizeof(short));
        }
    }
    for (int i = 0; i < lenBeforeArith_U; i++) {
        if (i < block_num) {
            num_s = (short)vec_DAR_U.at(i);
            outfile.write((char *)&num_s, sizeof(short));
        } else {
            num_uc = (unsigned char)vec_DAR_U.at(i);
            outfile.write((char *)&num_uc, sizeof(unsigned char));
            num_s = (short)vec_DAR_U.at(++i);
            outfile.write((char *)&num_s, sizeof(short));
        }
    }
    for (int i = 0; i < lenBeforeArith_V; i++) {
        if (i < block_num) {
            num_s = (short)vec_DAR_V.at(i);
            outfile.write((char *)&num_s, sizeof(short));
        } else {
            num_uc = (unsigned char)vec_DAR_V.at(i);
            outfile.write((char *)&num_uc, sizeof(unsigned char));
            num_s = (short)vec_DAR_V.at(++i);
            outfile.write((char *)&num_s, sizeof(short));
        }
    }
    
}


void WebP::readData() {
    filename = "test.bin";
    infile.open(filename, ios::binary);
    short h, w;
    infile.read((char *)&h, sizeof(short));
    infile.read((char *)&w, sizeof(short));
    width = w;
    height = h;
    int cols = width % MACROBLOCKSIZE == 0 ? width : (width / MACROBLOCKSIZE + 1) * MACROBLOCKSIZE;
    int rows = height % MACROBLOCKSIZE == 0 ? height : (height / MACROBLOCKSIZE + 1) * MACROBLOCKSIZE;
    block_cols = cols / MACROBLOCKSIZE;
    block_rows = rows / MACROBLOCKSIZE;
    readTypeData();
    readRunLengthCode();
    infile.close();
}

void WebP::readTypeData() {
    char buffer = 0;
    for (int i = 0; i <= block_cols * block_rows * 3 / 4; i++) {
        infile.read(&buffer, sizeof(char));
        reconstruct_type.push_back(BIT01(buffer));
        reconstruct_type.push_back(BIT23(buffer));
        reconstruct_type.push_back(BIT45(buffer));
        reconstruct_type.push_back(BIT67(buffer));
    }
}

void WebP::readArithmeticCode() {

}

void WebP::readRunLengthCode() {
    int block_num = block_cols * block_rows;
    short num_s;
    unsigned char num_uc;

    infile.read((char *)&lenBeforeArith_Y, sizeof(int));
    infile.read((char *)&lenBeforeArith_U, sizeof(int));
    infile.read((char *)&lenBeforeArith_V, sizeof(int));

    for (int i = 0; i < lenBeforeArith_Y; i++) {
        if (i < block_num) {
            infile.read((char *)&num_s, sizeof(short));
            ivec_DAR_Y.push_back(num_s);
        } else {
            infile.read((char *)&num_uc, sizeof(unsigned char));
            ivec_DAR_Y.push_back((short)num_uc);
            i ++;
            infile.read((char *)&num_s, sizeof(short));
            ivec_DAR_Y.push_back((short)num_s);
        }
    }
    for (int i = 0; i < lenBeforeArith_U; i++) {
        if (i < block_num) {
            infile.read((char *)&num_s, sizeof(short));
            ivec_DAR_U.push_back(num_s);
        } else {
            infile.read((char *)&num_uc, sizeof(unsigned char));
            ivec_DAR_U.push_back((short)num_uc);
            i ++;
            infile.read((char *)&num_s, sizeof(short));
            ivec_DAR_U.push_back((short)num_s);
        }
    }
    for (int i = 0; i < lenBeforeArith_V; i++) {
        if (i < block_num) {
            infile.read((char *)&num_s, sizeof(short));
            ivec_DAR_V.push_back(num_s);
        } else {
            infile.read((char *)&num_uc, sizeof(unsigned char));
            ivec_DAR_V.push_back((short)num_uc);
            i ++;
            infile.read((char *)&num_s, sizeof(short));
            ivec_DAR_V.push_back((short)num_s);
        }
    }
}


void WebP::initDctMat() {

    float dct_arr_8[8][8] = {
        0.35355338, 0.35355338, 0.35355338, 0.35355338, 0.35355338, 0.35355338, 0.35355338, 0.35355338, 
        0.49039263, 0.4157348, 0.27778512, 0.097545162, -0.097545162, -0.27778512, -0.4157348, -0.49039263, 
        0.46193975, 0.19134171, -0.19134171, -0.46193975, -0.46193975, -0.19134171, 0.19134171, 0.46193975, 
        0.4157348, -0.097545162, -0.49039263, -0.27778512, 0.27778512, 0.49039263, 0.097545162, -0.4157348, 
        0.35355338, -0.35355338, -0.35355338, 0.35355338, 0.35355338, -0.35355338, -0.35355338, 0.35355338, 
        0.27778512, -0.49039263, 0.097545162, 0.4157348, -0.4157348, -0.097545162, 0.49039263, -0.27778512, 
        0.19134171, -0.46193975, 0.46193975, -0.19134171, -0.19134171, 0.46193975, -0.46193975, 0.19134171, 
        0.097545162, -0.27778512, 0.4157348, -0.49039263, 0.49039263, -0.4157348, 0.27778512, -0.097545162
    };

    float dct_arr_16[16][16] = {
        0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25,
        0.35185093, 0.33832949, 0.31180626, 0.27330047, 0.22429189, 0.16666391, 0.10263113, 0.034654293, -0.034654293, -0.10263113, -0.16666391, -0.22429189, -0.27330047, -0.31180626, -0.33832949, -0.35185093, 
        0.34675995, 0.29396889, 0.19642374, 0.068974845, -0.068974845, -0.19642374, -0.29396889, -0.34675995, -0.34675995, -0.29396889, -0.19642374, -0.068974845, 0.068974845, 0.19642374, 0.29396889, 0.34675995, 
        0.33832949, 0.22429189, 0.034654293, -0.16666391, -0.31180626, -0.35185093, -0.27330047, -0.10263113, 0.10263113, 0.27330047, 0.35185093, 0.31180626, 0.16666391, -0.034654293, -0.22429189, -0.33832949, 
        0.32664073, 0.13529903, -0.13529903, -0.32664073, -0.32664073, -0.13529903, 0.13529903, 0.32664073, 0.32664073, 0.13529903, -0.13529903, -0.32664073, -0.32664073, -0.13529903, 0.13529903, 0.32664073, 
        0.31180626, 0.034654293, -0.27330047, -0.33832949, -0.10263113, 0.22429189, 0.35185093, 0.16666391, -0.16666391, -0.35185093, -0.22429189, 0.10263113, 0.33832949, 0.27330047, -0.034654293, -0.31180626, 
        0.29396889, -0.068974845, -0.34675995, -0.19642374, 0.19642374, 0.34675995, 0.068974845, -0.29396889, -0.29396889, 0.068974845, 0.34675995, 0.19642374, -0.19642374, -0.34675995, -0.068974845, 0.29396889, 
        0.27330047, -0.16666391, -0.33832949, 0.034654293, 0.35185093, 0.10263113, -0.31180626, -0.22429189, 0.22429189, 0.31180626, -0.10263113, -0.35185093, -0.034654293, 0.33832949, 0.16666391, -0.27330047, 
        0.25, -0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25,
        0.22429189, -0.31180626, -0.10263113, 0.35185093, -0.034654293, -0.33832949, 0.16666391, 0.27330047, -0.27330047, -0.16666391, 0.33832949, 0.034654293, -0.35185093, 0.10263113, 0.31180626, -0.22429189, 
        0.19642374, -0.34675995, 0.068974845, 0.29396889, -0.29396889, -0.068974845, 0.34675995, -0.19642374, -0.19642374, 0.34675995, -0.068974845, -0.29396889, 0.29396889, 0.068974845, -0.34675995, 0.19642374, 
        0.16666391, -0.35185093, 0.22429189, 0.10263113, -0.33832949, 0.27330047, 0.034654293, -0.31180626, 0.31180626, -0.034654293, -0.27330047, 0.33832949, -0.10263113, -0.22429189, 0.35185093, -0.16666391, 
        0.13529903, -0.32664073, 0.32664073, -0.13529903, -0.13529903, 0.32664073, -0.32664073, 0.13529903, 0.13529903, -0.32664073, 0.32664073, -0.13529903, -0.13529903, 0.32664073, -0.32664073, 0.13529903, 
        0.10263113, -0.27330047, 0.35185093, -0.31180626, 0.16666391, 0.034654293, -0.22429189, 0.33832949, -0.33832949, 0.22429189, -0.034654293, -0.16666391, 0.31180626, -0.35185093, 0.27330047, -0.10263113, 
        0.068974845, -0.19642374, 0.29396889, -0.34675995, 0.34675995, -0.29396889, 0.19642374, -0.068974845, -0.068974845, 0.19642374, -0.29396889, 0.34675995, -0.34675995, 0.29396889, -0.19642374, 0.068974845, 
        0.034654293, -0.10263113, 0.16666391, -0.22429189, 0.27330047, -0.31180626, 0.33832949, -0.35185093, 0.35185093, -0.33832949, 0.31180626, -0.27330047, 0.22429189, -0.16666391, 0.10263113, -0.03465429, 
    };

    dct_mat_16 = Mat(MACROBLOCKSIZE, MACROBLOCKSIZE, CV_32F, dct_arr_16).clone();
    dct_mat_8 = Mat(MACROBLOCKSIZE / 2, MACROBLOCKSIZE / 2, CV_32F, dct_arr_8).clone();
    dct_mat_16_T = dct_mat_16.t();
    dct_mat_8_T = dct_mat_8.t();
}

Mat WebP::mydct(Mat input_mat) {

    Mat dct_mat, dct_mat_T;
    if (input_mat.cols == 16 && input_mat.rows == 16) {
        dct_mat = dct_mat_16;
        dct_mat_T = dct_mat_16_T;
    } else if(input_mat.cols == 8 && input_mat.rows == 8) {
        dct_mat = dct_mat_8;
        dct_mat_T = dct_mat_8_T;
    } else {
        cout<<"[ERROR]: dct wrong input, the size of input is wrong!"<<endl;
    }
    input_mat.convertTo(input_mat, CV_32F);
    return dct_mat * input_mat * dct_mat_T;
}

Mat WebP::myidct(Mat input_mat) {    
    Mat dct_mat, dct_mat_T;
    if (input_mat.cols == 16 && input_mat.rows == 16) {
        dct_mat = dct_mat_16;
        dct_mat_T = dct_mat_16_T;
    } else if(input_mat.cols == 8 && input_mat.rows == 8) {
        dct_mat = dct_mat_8;
        dct_mat_T = dct_mat_8_T;
    } else {
        cout<<"[ERROR]: dct wrong input, the size of input is wrong!"<<endl;
    }
    input_mat.convertTo(input_mat, CV_32F);
    return dct_mat_T * input_mat * dct_mat;
}


