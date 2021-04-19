#include "WebP.h"

WebP::WebP(String path) {
    this->path = path;

    test();
    compress();
    
    idct_Y = dct_Y.clone();
    idct_U = dct_U.clone();
    idct_V = dct_V.clone();
    reconstruct_type.assign(predict_type.begin(), predict_type.end());
    uncompress();
    imshow("test", img_reconstruct);
    waitKey();
}

void WebP::test() {
    imagePretreatment(path);
    predict_type.clear();
    subSampling();
    int block_num;
    Mat temp0, temp1, temp2;
    Mat y_pre, u_pre, v_pre, yn, un, vn;
    y_pre = Y.clone();
    u_pre = U.clone();
    v_pre = V.clone();

    yn.create(Y.rows, Y.cols, CV_16S);
    un.create(U.rows, U.cols, CV_16S);
    vn.create(V.rows, V.cols, CV_16S);

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

            cout<<temp0<<endl;

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

            cout<<temp0<<endl;

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
            temp0 = inpredictCoding(block_num, MACROBLOCKSIZE, yn, y_pre(Range(i * MACROBLOCKSIZE, (i + 1) * MACROBLOCKSIZE), Range(j * MACROBLOCKSIZE, (j + 1) * MACROBLOCKSIZE)));
            temp1 = inpredictCoding(block_num, MACROBLOCKSIZE / 2, un, u_pre(Range(i * MACROBLOCKSIZE / 2, (i + 1) * MACROBLOCKSIZE / 2), Range(j * MACROBLOCKSIZE / 2, (j + 1) * MACROBLOCKSIZE / 2)));
            temp2 = inpredictCoding(block_num, MACROBLOCKSIZE / 2, vn, v_pre(Range(i * MACROBLOCKSIZE / 2, (i + 1) * MACROBLOCKSIZE / 2), Range(j * MACROBLOCKSIZE / 2, (j + 1) * MACROBLOCKSIZE / 2)));

            temp0.copyTo(yn(Range(i * MACROBLOCKSIZE, (i + 1) * MACROBLOCKSIZE), Range(j * MACROBLOCKSIZE, (j + 1) * MACROBLOCKSIZE)));
            temp1.copyTo(un(Range(i * MACROBLOCKSIZE / 2, (i + 1) * MACROBLOCKSIZE / 2), Range(j * MACROBLOCKSIZE / 2, (j + 1) * MACROBLOCKSIZE / 2)));
            temp2.copyTo(vn(Range(i * MACROBLOCKSIZE / 2, (i + 1) * MACROBLOCKSIZE / 2), Range(j * MACROBLOCKSIZE / 2, (j + 1) * MACROBLOCKSIZE / 2)));

        }
    }
    Mat YUVn;
    YUVn.create(Y.rows, Y.cols, CV_16SC3);
    for (int i = 0; i < YUVn.rows; i++) {
        for (int j = 0; j < YUVn.cols; j++) {
            YUVn.at<Vec3s>(i, j)[0] = yn.at<short>(i, j);
            YUVn.at<Vec3s>(i, j)[1] = un.at<short>(i / 2, j / 2);
            YUVn.at<Vec3s>(i, j)[2] = vn.at<short>(i / 2, j / 2);
        }
    }
    YUVn.convertTo(temp0, CV_8UC3);
    Mat test;
    cvtColor(temp0, test, COLOR_YUV2BGR);
    imshow("drrd", test);
    yn.convertTo(temp0, CV_8U);
    imshow("dd", temp0);
    vn.convertTo(temp0, CV_8U);
    imshow("ddf", temp0);
    un.convertTo(temp0, CV_8U);
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
    cvtColor(img_origin, img_YUV, COLOR_RGB2YUV);
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
    residual_mat.convertTo(residual_mat, CV_32F);
    cv::dct(residual_mat, result_mat);
    //cout<<residual_mat<<endl;
    //cout<<result_mat<<endl;
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
    idct(indct_mat, residual_mat);
    reconstruct_block_mat = inpredictCoding(block_num, block_size, reconstruct_mat, residual_mat);    
    Mat temp_mat = reconstruct_mat(Range(block_row * block_size, block_row * block_size + block_size), Range(block_col * block_size, block_col * block_size + block_size));
    reconstruct_block_mat.copyTo(temp_mat);
    return reconstruct_block_mat;
}
Mat WebP::inpredictCoding(int block_num, int block_size, Mat channel_mat, Mat residual_mat) {
    int type = reconstruct_type.at(block_num);
    Mat reconstruct_mat;
    switch (type) {
        case H_PRED :
            reconstruct_mat = inhPredict(block_num, block_size, channel_mat, residual_mat);
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
    cvtColor(YUV_reconstruct, img_reconstruct_16base, COLOR_YUV2BGR);
    img_reconstruct = Mat::zeros(height, width, CV_16S);
    (img_reconstruct_16base(Range(0, height), Range(0, width))).copyTo(img_reconstruct);
    return YUV_reconstruct;
}
void WebP::uncompress() {

    //idct_Y.create( block_cols * block_rows, MACROBLOCKSIZE * MACROBLOCKSIZE, CV_16S);
    //idct_U.create( block_cols * block_rows, MACROBLOCKSIZE / 2 * MACROBLOCKSIZE / 2, CV_16S);
    //idct_V.create( block_cols * block_rows, MACROBLOCKSIZE / 2 * MACROBLOCKSIZE / 2, CV_16S);

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