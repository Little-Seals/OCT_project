#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QMessageBox>
class Model;
class OCTGUI;
using namespace cv;
using namespace std;

class WindowDisplay
{
public:
    Model* model;
    OCTGUI* view;
    int sliderValue;
    QString windowTitle;

    ~WindowDisplay()
    {

    }
    void displayBScanImage(float* imageData, int width, int height,
        const string& windowTitle, int yIndex, bool usePseudocolor);

    void BscanImagePocessor(Model* model, OCTGUI* view, QString windowTitle,
        int sliderValue);

    void BscanImagePocessor(Model* model, OCTGUI* view, QString windowTitle, int sliderValue
        , bool isApplyMask);

    void displayIntensityImage(Model* model, OCTGUI* view, int sliderValue);


    // 新增：显示强度掩膜图像
    void displayIntensityMaskImage(Model* model, OCTGUI* view, int sliderValue);
};


