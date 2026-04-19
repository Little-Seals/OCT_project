#pragma once

#include<QObject>           // Qt对象基类，支持信号槽机制
#include"opencv2/opencv.hpp" // OpenCV计算机视觉库
#include"SpectralRadar.h"   // 光谱雷达处理相关头文件（可能是自定义库）
#include<string>            // C++字符串处理
#include<vector>            // C++动态数组容器
#include<complex>           // C++复数运算支持
#include<qDebug>            // Qt调试输出
#include<fstream>           // 文件流操作
#include<chrono>            // C++时间库（用于计时等）
using namespace std;


class Model : public QObject {
    Q_OBJECT  

public:
    
    explicit Model(QObject* parent = nullptr);

    void getIntensityImage(QString& filePath);
    void loadAndProcessOCTRawData(const QString& fileName);
    bool OnError(void);
    void OCTA_algorithm_complex_correlaton();

    float* compute_OCTA(int m, int n, int interval);
    void compute_Intensity_mask(float threshold);

public:
    
    float Z_range;  // 深度方向扫描范围（Z轴）
    float X_range;  // 横向扫描范围（X轴）  
    float Y_range;  // 纵向扫描范围（Y轴）
    int Z_pixel;    // 深度方向像素数（A扫描点数）
    int X_pixel;    // 横向像素数（B扫描宽度）注意：变量名拼写错误应为X_pixel
    int Y_pixel;    // 纵向像素数（B扫描数量）注意：变量名拼写错误应为Y_pixel
    int Ascan_repeat;  // A扫描重复次数（用于降噪）
    int Bscan_repeat;  // B扫描重复次数（用于运动分析）
    std::string Scan_mode;
    
    double Threshold;
    
    float* Intensity_ptr;
    std::vector<std::complex<float>> complex_data;

    float* liv_ptr;          // LIV算法结果指针
    float* octa_result_ptr;  // OCTA算法结果指针
    uint8_t* oct_intensity_mask;

};

