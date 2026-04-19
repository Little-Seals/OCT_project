
#ifdef MAIN2t



#include <iostream>
#include "cuhelper.cuh"
#include "cufinufft.h"
#include <opencv2/opencv.hpp>
int main(int argc, char* argv[]) //CPU版本
{
    const int RawAsize = 2048;
    const int RawBsize = 1024;
    float* ApoData = new float[RawAsize]{ 0 }; //DC文件
    float* ChirpData = new float[RawAsize]{ 0 }; //插值文件
    uint16_t* RawData = new uint16_t[RawAsize * RawBsize]{ 0 }; //光谱文件
    complex<float>* ComplexData = new complex<float>[RawAsize * RawBsize]{ 0 }; //复光谱文件
    /* 步骤一 ------------------------------*/
    //读取Apo文件
    FILE* fp = fopen("./ApodizationSpectrum.data", "rb");
    if (!fp) { printf("ApodizationSpectrum.data ->失败，请退出检查\r\n"); system("pause"); return 1; }
    size_t read = fread(ApoData, sizeof(float), RawAsize, fp);
    printf("ApoData: %zu floats\n", read);
    fclose(fp);
    //读取Chirp文件
    fp = fopen("./Chirp.data", "rb");
    if (!fp) { printf("ChirpData.raw ->失败，请退出检查\r\n"); system("pause"); return 1; }
    read = fread(ChirpData, sizeof(float), RawAsize, fp);
    printf("ChirpData: %zu floats\n", read);
    fclose(fp);
    //读取光谱文件
    fp = fopen("./Spectral0.data", "rb");
    if (!fp) { printf("Spectral0.data ->失败，请退出检查\r\n"); system("pause"); return 1; }
    fseek(fp, 25 * RawAsize * sizeof(uint16_t), SEEK_SET);
    read = fread(RawData, sizeof(uint16_t), RawAsize * RawBsize, fp);
    printf("RawData: %zu uint32_ts\n", read);
    fclose(fp);
    /* 步骤二 ------------------------------*/
    for (int i = 0; i < RawAsize; i++) {
        ChirpData[i] = M_PI * (ChirpData[i] / 1024.0 - 1.0f);
    }
    for (int i = 0; i < RawAsize * RawBsize; i++) {
        ComplexData[i].real(34.179688 * RawData[i]);
        ComplexData[i].imag(0.00f);
    }
    for (int i = 0; i < RawBsize; i++) { //列
        for (int j = 0; j < RawAsize; j++) { //行
            float x = ComplexData[i * 2048 + j].real();
            ComplexData[i * 2048 + j].real(x - ApoData[j]);
        }
    }

    FLT* d_x;
    CUCPX* d_c, * d_fk;
    checkCudaErrors(cudaMalloc(&d_x, RawAsize * sizeof(FLT)));
    checkCudaErrors(cudaMalloc(&d_c, RawAsize * RawBsize * sizeof(CUCPX)));
    checkCudaErrors(cudaMalloc(&d_fk, RawAsize * RawBsize * sizeof(CUCPX)));

    checkCudaErrors(cudaMemcpy(d_x, ChirpData, RawAsize * sizeof(FLT), cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(d_c, ComplexData, RawAsize * RawBsize * sizeof(CPX), cudaMemcpyHostToDevice));

    cufinufftf_plan dplan;
    cufinufft_opts opts;
    int ier = cufinufftf_default_opts(1, 1, &opts);
    opts.gpu_method = 1;
    int maxbatchsize = 8;
    int nmodes[3] = { RawAsize,1,1 };

    int ntransf = RawBsize;

    ier = cufinufftf_makeplan(1, 1, nmodes, 1, ntransf, 1e-6, maxbatchsize, &dplan, &opts); printf("ier=%d\n", ier);

    //cudaDeviceSynchronize();
    ier = cufinufftf_setpts(RawAsize, d_x, NULL, NULL, 0, NULL, NULL, NULL, dplan); printf("ier=%d\n", ier);
    //cudaDeviceSynchronize();

    float milliseconds = 0;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    //执行NUFFT
    cudaEventRecord(start);
    ier = cufinufftf_execute(d_c, d_c, dplan);
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&milliseconds, start, stop);
    //打印时间
    printf("[time] cufinufft plan:\t\t %.3g s\n", milliseconds / 1000);
    //printf("ier=%d\n", ier);
    ier = cufinufftf_destroy(dplan); printf("ier=%d\n", ier);
    //cudaDeviceSynchronize();
    cudaMemcpy(ComplexData, d_c, RawAsize * RawBsize * sizeof(CUCPX), cudaMemcpyDeviceToHost);

    float* xData = new float[RawAsize * RawBsize]{ 0 }; //光谱文件
    for (int i = 0; i < 2048 * 1024; i++) {
        xData[i] = 20 * log(1 + abs(ComplexData[i]));
    }
    float max = -1000.0f, min = 1000.0f;
    for (int i = 0; i < 2048 * 1024; i++) {
        if (xData[i] > max)max = xData[i];
        if (xData[i] < min)min = xData[i];
    }
    printf("[min,max]=[%f,%f]\n", min, max);
    for (int i = 0; i < 2048 * 1024; i++) {
        xData[i] = (xData[i] - min) / (max - min);
    }
    max = -1000.0f, min = 1000.0f;
    for (int i = 0; i < 2048 * 1024; i++) {
        if (xData[i] > max)max = xData[i];
        if (xData[i] < min)min = xData[i];
    }
    printf("[min,max]=[%f,%f]\n", min, max);
    //把fk转换成OpenCv的 Mat进行显示
    cv::Mat img = cv::Mat::zeros(cv::Size(1024, 1024), CV_32FC1);
    for (int i = 0; i < 1024; i++) {
        for (int j = 0; j < 1024; j++) {
            img.at<float>(j, i) = xData[i * 2048 + j + 1024];
        }
    }
    cv::imshow("result", img);
    cv::waitKey(0);
    return 0;
    ////读取图片
    //cv::Mat img = imread("./Li.jpg", cv::IMREAD_UNCHANGED);//以原图格式读取
    //if (img.empty()) {
    //    std::cout << "加载图片失败" << std::endl;
    //}
    //else {
    //    imshow("窗口1", img);  //在窗口1中显示图片img(窗口大小不能调整)
    //}
    //cv::waitKey(0);        //显示图片的窗口停顿
    ////waitKey(2000);       //显示图片的窗口停顿2000ms
    ////destroyAllWindows(); //销毁所有的窗口
    return 0;
}


#endif