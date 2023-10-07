#include "bilateral_separable_conv.hpp"

MyBilateralSepConvTest::MyBilateralSepConvTest() {
}

MyBilateralSepConvTest::~MyBilateralSepConvTest() {
}

Mat MyBilateralSepConvTest::CalGaussianTemplate(int r, float sigma) {
    float pi = 3.1415926;
    int center = r;
    int ksize = r*2+1;
    float x2;

    Mat Kore = Mat::zeros(Size(ksize, 1), CV_32FC1);
    for (int i = 0; i < ksize; i++) {
        x2 = pow(i - center, 2);
        double g = exp(-(x2) / (2 * sigma * sigma));
        g /= 2 * pi * sigma;
        Kore.at<float>(0, i) = g;
    }

    return Kore;
}

vector<float> MyBilateralSepConvTest::CalValueTemplate(float sigma) {
    vector<float> val_weight_arr;

    for(int i=0; i<256; i++) {
        float cur_weight = exp(-(i*i) / (2 * sigma * sigma));
        val_weight_arr.push_back(cur_weight);
    }

    return val_weight_arr;
}
    
Mat MyBilateralSepConvTest::BilateralBlur(Mat src, Mat gaussian_kore, vector<float> val_weight_arr, int r) {
    Mat out = Mat::zeros(src.size(), src.type());

    for (int i=r; i<src.rows-r; i++) {
        for (int j=r; j<src.cols-r; j++) {
            float cur_weight = 0.0;
            float value = 0.0;
            for (int m=-r; m<=r; m++){
                float weight = gaussian_kore.at<float>(0, m+r) 
                    * val_weight_arr[abs(src.at<uchar>(i,j) - src.at<uchar>(i+m,j))];
                value += src.at<uchar>(i+m,j) * weight;
                cur_weight += weight;
            }
            value = value / cur_weight;
            out.at<uchar>(i, j) = value;
        }
    }

    Mat out2 = out.clone();

    for (int i=r; i<src.rows-r; i++) {
        for (int j=r; j<src.cols-r; j++) {
            float cur_weight = 0.0;
            float value = 0.0;
            for (int n=-r; n<=r; n++){
                float weight = gaussian_kore.at<float>(0, n+r) 
                    * val_weight_arr[abs(src.at<uchar>(i,j) - src.at<uchar>(i,j+n))];
                value += out.at<uchar>(i,j+n) * weight;
                cur_weight += weight;
            }
            value = value / cur_weight;
            out2.at<uchar>(i, j) = value;
        }
    }

    return out2;
}

Mat MyBilateralSepConvTest::Run(Mat src, int r, float gauss_sigma, float value_sigma) {
    Mat gaussian_kore = CalGaussianTemplate(r, gauss_sigma);
    vector<float> val_weight_arr = CalValueTemplate(value_sigma);

    Mat bilateral_blur = BilateralBlur(src, gaussian_kore, val_weight_arr, r);

	return bilateral_blur;
}
