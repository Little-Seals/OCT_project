#pragma once
#include "iostream"
#include "opencv2/opencv.hpp"
#include"SpectralRadar.h"
using namespace cv;
using namespace std;

class DecorrelationAlgorithm {
public:
    vector<float> conv2_same_output;
    vector<float> decorrelation_result;
    int X;
    int Z;
    int m;
    int n;

    DecorrelationAlgorithm(int X, int Z, int m, int n);


    void conv2_same(const vector<float>& input, const vector<float>& kernel);


    void compute_complex_decorrelation(const vector<complex<float>>& A, const vector<complex<float>>& B);
};