#include "cvshow.cuh"
using namespace cv;
void cvshow(float* ObjImage, uint32_t ObjAsize, uint32_t ObjBsize, uint8_t flag) {
    float max_val = -1000.0f, min_val = 1000.0f;
    if (flag) {
        printf("=>执行 20 * log10 !!!\n");
        for (int i = 0; i < ObjAsize * ObjBsize; i++) {
            ObjImage[i] = 20 * log10(ObjImage[i]);
        }
    }
    for (int i = 0; i < ObjAsize * ObjBsize; i++) {
        if (ObjImage[i] > max_val)max_val = ObjImage[i];
        if (ObjImage[i] < min_val)min_val = ObjImage[i];
    }
    printf("[min,max]=[%f,%f]\n", min_val, max_val);
    if ((1.0f < max_val) || (min_val < 0.00f)) {
        for (int i = 0; i < ObjAsize * ObjBsize; i++) {
            ObjImage[i] = (ObjImage[i] - min_val) / (max_val - min_val);
        }
    }

    FILE* fp = fopen("data.bin", "wb");
    fwrite(ObjImage, sizeof(float), ObjAsize * ObjBsize, fp);
    fclose(fp);

    //把fk转换成OpenCv的 Mat进行显示
    cv::Mat img = cv::Mat::zeros(cv::Size(1024, 1024), CV_32FC1);
    for (int i = 0; i < ObjBsize; i++) {
        for (int j = 0; j < ObjAsize; j++) {
                img.at<float>(j, i) = ObjImage[i * ObjAsize + j]; //上半部分
        }
    }
    Mat gray_img;
    img.convertTo(gray_img, CV_8UC1, 255);
    Mat colormap;
    applyColorMap(gray_img, colormap, COLORMAP_PARULA);
    namedWindow("ObjImg", WINDOW_NORMAL);
    cv::imshow("ObjImg", colormap);
    cv::waitKey(0);
}