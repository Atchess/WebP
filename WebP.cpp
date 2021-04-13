#include "WebP.h"

WebP::WebP(String path) {
    imagePretreatment(path);
}

WebP::~WebP() {

}

void WebP::imagePretreatment(String path) {
    img_origin = imread(path);
    Mat img_YUV;
    cvtColor(img_origin, img_YUV, COLOR_RGB2YUV);
    if (img_YUV.cols % BLOCKSIZE != 0) {
        int remainder = img_YUV.cols % BLOCKSIZE;
    } else {

    }
    if (img_YUV.rows % BLOCKSIZE != 0) {

    }
}
