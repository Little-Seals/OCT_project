#include "windowDisplay.h"
#include "model.h"    // 现在在这里包含
#include "OCTGUI.h"     // 现在在这里包含





/**
 * @brief 显示B扫描图像（公共显示逻辑）
 * @param imageData 图像数据指针
 * @param width 图像宽度（X方向像素数）
 * @param height 图像高度（Z方向像素数）
 * @param windowTitle 窗口标题
 * @param yIndex 当前显示的Y索引（用于标题显示）
 * @param usePseudocolor 是否使用伪彩色显示（默认为false）
 */
void WindowDisplay::displayBScanImage(float* imageData, int width, int height,
    const string& windowTitle, int yIndex, bool usePseudocolor) {
    if (imageData == nullptr || width <= 0 || height <= 0) {
        cout << "错误：图像数据无效或尺寸错误" << endl;
        return;
    }

    // 创建图像矩阵
    Mat bscanImage(width, height, CV_32FC1);
    memcpy(bscanImage.ptr<float>(0), imageData, width * height * sizeof(float));

    // 归一化图像数据以便显示
    normalize(bscanImage, bscanImage, 0, 255, NORM_MINMAX, CV_8UC1);

    // 转置图像使其方向正确
    transpose(bscanImage, bscanImage);

    // 伪彩色处理
    Mat displayImage;
    stringstream ss;
    if (usePseudocolor) {
        // 应用伪彩色映射（使用Jet色图）
        applyColorMap(bscanImage, displayImage, COLORMAP_JET);
        cout << "使用伪彩色显示模式" << endl;
        // 构建窗口标题
        ss << windowTitle << (usePseudocolor ? " [伪彩色]" : " [灰度]");
    }
    else {
        // 灰度显示
        cvtColor(bscanImage, displayImage, COLOR_GRAY2BGR);
        cout << "使用灰度显示模式" << endl;
        // 构建窗口标题
        ss << windowTitle << " (Y=" << yIndex << ")" << (usePseudocolor ? " [伪彩色]" : " [灰度]");
    }

    
    string windowName = ss.str();

    // 创建可调整大小的窗口
    namedWindow(windowName, WINDOW_NORMAL);

    // 设置固定窗口大小为500x500
    resizeWindow(windowName, 500, 500);

    // 窗口居中显示（可选，根据需要保留）
    int posX = (1920 - 500) / 2;  // 假设屏幕宽度为1920
    int posY = (1080 - 500) / 2;  // 假设屏幕高度为1080
    moveWindow(windowName, posX, posY);

    imshow(windowName, displayImage);
    cout << "窗口大小已固定为: 500 x 500" << endl;
    cout << "正在显示图像，按任意键关闭窗口..." << endl;

    waitKey(0);
    destroyAllWindows();
}


void WindowDisplay::BscanImagePocessor(Model* model, OCTGUI* view, QString windowTitle,
    int sliderValue)
{
    this->windowTitle = windowTitle;
    QLabel* view_label = nullptr;

    if (this->windowTitle == "OCTAView") {
        view_label = view->ui.OCTA_figure;
        // OCTA 图像处理逻辑
        if (model->octa_result_ptr == nullptr || model->oct_intensity_mask == nullptr) {
            qDebug() << "Error: OCTA data or mask not available";
            return;
        }

        cv::Mat octa_mat(model->X_pixel, model->Z_pixel, CV_32FC1);
        float* bscan_ptr = new float[model->X_pixel * model->Z_pixel];

        int yIndex = sliderValue;
        int frameSize = model->X_pixel * model->Z_pixel;
        int dataOffset = yIndex * frameSize;
            
        // 根据强度掩码过滤数据：有效区域保留结果，无效区域置零
        for (int i = 0; i < frameSize; i++) {
             int globalIndex = i + dataOffset;
             if (model->oct_intensity_mask[globalIndex] > 0) {
                 bscan_ptr[i] = model->octa_result_ptr[globalIndex];
             }
             else {
                 bscan_ptr[i] = 0;
            }
        }


        memcpy(octa_mat.ptr<float>(0), bscan_ptr, frameSize * sizeof(float));
        delete[] bscan_ptr;

        // 对图像进行归一化处理，转换为 8 位单通道图像
        cv::normalize(octa_mat, octa_mat, 0, 255, cv::NORM_MINMAX, CV_8UC1);

        // 转置图像矩阵以适应显示方向
        cv::transpose(octa_mat, octa_mat);

        // 应用伪彩色映射，根据用户选择的颜色映射方式
        cv::Mat colorImage;
        cv::applyColorMap(octa_mat, colorImage, view->ui.comboBox->currentData().toInt());

        // 将 OpenCV 的图像转换为 QImage，以便在 Qt 界面中显示
        QImage qimg(
            colorImage.data,
            colorImage.cols,
            colorImage.rows,
            colorImage.step,
            QImage::Format_BGR888
        );

        // 设置图像到界面控件中，并进行缩放显示
        view_label->setPixmap(
            QPixmap::fromImage(qimg).scaled(
                view_label->size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
            )
        );
    }
    else if (this->windowTitle == "RawDataView")
    {
        view_label = view->ui.imageLabel;

        // 原有的 B-scan 强度图像处理逻辑
        cv::Mat bscanImage(model->X_pixel, model->Z_pixel, CV_32FC1);
        float* oct_intensity = new float[model->Z_pixel * model->X_pixel];

        int yIndex = sliderValue * model->Bscan_repeat;
        for (int i = 0; i < model->Z_pixel * model->X_pixel; i++) {
            int data_index = i + model->Z_pixel * model->X_pixel * yIndex;
            float magnitude = std::abs(model->complex_data[data_index]);
            oct_intensity[i] = float(20.0 * std::log10(magnitude + 10e-6f));
        }

        memcpy(bscanImage.ptr<float>(0), oct_intensity, model->X_pixel * model->Z_pixel * sizeof(float));
        delete[] oct_intensity;

        cv::normalize(bscanImage, bscanImage, 0, 255, cv::NORM_MINMAX, CV_8UC1);
        cv::transpose(bscanImage, bscanImage);

        QImage qimg(bscanImage.data, bscanImage.cols, bscanImage.rows, bscanImage.step, QImage::Format_Grayscale8);
        view_label->setPixmap(
            QPixmap::fromImage(qimg).scaled(
                view_label->size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
            )
        );
    }
}


void WindowDisplay::BscanImagePocessor(Model* model, OCTGUI* view, QString windowTitle, 
    int sliderValue, bool isApplyMask)
{
    this->windowTitle = windowTitle;
    QLabel* view_label = nullptr;

    if (this->windowTitle == "OCTAView") {
        view_label = view->ui.OCTA_figure;
        // OCTA 图像处理逻辑
        if (model->octa_result_ptr == nullptr || model->oct_intensity_mask == nullptr) {
            qDebug() << "Error: OCTA data or mask not available";
            return;
        }

        cv::Mat octa_mat(model->X_pixel, model->Z_pixel, CV_32FC1);
        float* bscan_ptr = new float[model->X_pixel * model->Z_pixel];

        int yIndex = sliderValue;
        int frameSize = model->X_pixel * model->Z_pixel;
        int dataOffset = yIndex * frameSize;

        if (isApplyMask)
        {
            // 根据强度掩码过滤数据：有效区域保留结果，无效区域置零
            for (int i = 0; i < frameSize; i++) {
                int globalIndex = i + dataOffset;
                if (model->oct_intensity_mask[globalIndex] > 0) {
                    bscan_ptr[i] = model->octa_result_ptr[globalIndex];
                }
                else {
                    bscan_ptr[i] = 0;
                }
            }
        }
        else
        {
            for (int i = 0; i < frameSize; i++) {
                int globalIndex = i + dataOffset;
                bscan_ptr[i] = model->octa_result_ptr[globalIndex];
            }
        }
        

        memcpy(octa_mat.ptr<float>(0), bscan_ptr, frameSize * sizeof(float));
        delete[] bscan_ptr;

        // 对图像进行归一化处理，转换为 8 位单通道图像
        cv::normalize(octa_mat, octa_mat, 0, 255, cv::NORM_MINMAX, CV_8UC1);

        // 转置图像矩阵以适应显示方向
        cv::transpose(octa_mat, octa_mat);

        // 应用伪彩色映射，根据用户选择的颜色映射方式
        cv::Mat colorImage;
        cv::applyColorMap(octa_mat, colorImage, view->ui.comboBox->currentData().toInt());

        // 将 OpenCV 的图像转换为 QImage，以便在 Qt 界面中显示
        QImage qimg(
            colorImage.data,
            colorImage.cols,
            colorImage.rows,
            colorImage.step,
            QImage::Format_BGR888
        );

        // 设置图像到界面控件中，并进行缩放显示
        view_label->setPixmap(
            QPixmap::fromImage(qimg).scaled(
                view_label->size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
            )
        );
    }
    else if (this->windowTitle == "RawDataView")
    {
        view_label = view->ui.imageLabel;

        // 原有的 B-scan 强度图像处理逻辑
        cv::Mat bscanImage(model->X_pixel, model->Z_pixel, CV_32FC1);
        float* oct_intensity = new float[model->Z_pixel * model->X_pixel];

        int yIndex = sliderValue;
        for (int i = 0; i < model->Z_pixel * model->X_pixel; i++) {
            int data_index = i + model->Z_pixel * model->X_pixel * yIndex;
            float magnitude = std::abs(model->complex_data[data_index]);
            oct_intensity[i] = float(20.0 * std::log10(magnitude + 10e-6f));
        }

        memcpy(bscanImage.ptr<float>(0), oct_intensity, model->X_pixel * model->Z_pixel * sizeof(float));
        delete[] oct_intensity;

        cv::normalize(bscanImage, bscanImage, 0, 255, cv::NORM_MINMAX, CV_8UC1);
        cv::transpose(bscanImage, bscanImage);

        QImage qimg(bscanImage.data, bscanImage.cols, bscanImage.rows, bscanImage.step, QImage::Format_Grayscale8);
        view_label->setPixmap(
            QPixmap::fromImage(qimg).scaled(
                view_label->size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
            )
        );
    }
}


/**
 * @brief 显示指定Y截面的强度图像
 *
 * 该函数根据传入的滑块值选取OCT数据中对应的Y截面，并将其以灰度图形式显示在界面指定控件上。
 * 包含完整的安全检查、数据有效性验证、图像归一化与格式转换流程。
 *
 * @param model 指向Model结构体的指针，包含图像尺寸信息和强度数据指针
 * @param view 指向OCTGUI对象的指针，用于访问UI组件
 * @param sliderValue 当前Y轴切片索引，由用户通过滑块输入
 */
void WindowDisplay::displayIntensityImage(Model* model, OCTGUI* view, int sliderValue)
{
    // 安全检查：确保模型和强度数据有效
    if (model == nullptr || model->Intensity_ptr == nullptr) {
        qDebug() << "错误：模型或强度数据未初始化";
        return;
    }

    if (model->X_pixel <= 0 || model->Z_pixel <= 0 || model->Y_pixel < 0) {
        qDebug() << "错误：图像尺寸参数无效";
        return;
    }

    // 验证滑块值在有效范围内
    int yIndex = sliderValue;
    if (yIndex < 0 || yIndex >= model->Y_pixel) {
        qDebug() << "警告：滑块值超出范围，使用默认值0";
        yIndex = 0;
    }

    // 计算当前Y截面在数据中的起始索引
    int sliceSize = model->X_pixel * model->Z_pixel;
    int startIndex = yIndex * sliceSize;

    // 安全检查数据索引
    if (startIndex + sliceSize > model->Y_pixel * sliceSize) {
        qDebug() << "错误：数据索引超出范围";
        return;
    }

    // 直接使用强度数据，不需要对数变换（因为Intensity_ptr已经是处理好的强度数据）
    cv::Mat intensityImage(model->X_pixel, model->Z_pixel, CV_32FC1,
        model->Intensity_ptr + startIndex);

    // 图像后处理：归一化到0-255范围
    cv::Mat normalizedImage;
    cv::normalize(intensityImage, normalizedImage, 0, 255, cv::NORM_MINMAX, CV_8UC1);

    // 转置矩阵使图像方向正确
    cv::transpose(normalizedImage, normalizedImage);

    // 将OpenCV矩阵转换为Qt的QImage格式
    QImage qimg(normalizedImage.data, normalizedImage.cols, normalizedImage.rows,
        normalizedImage.step, QImage::Format_Grayscale8);

    // 关键修改：使用正确的显示控件
    // 假设你有一个专门用于显示强度图像的label，比如intensityImageLabel
    QLabel* targetLabel = view->ui.imageLabel; // 或者 view->ui.imageLabel


    // 更新界面上的图像显示
    QPixmap pixmap = QPixmap::fromImage(qimg);
    QSize labelSize = targetLabel->size();

    // 缩放图像以适应label大小
    QPixmap scaledPixmap = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    targetLabel->setPixmap(scaledPixmap);
    targetLabel->setAlignment(Qt::AlignCenter);
    targetLabel->setScaledContents(true);

    qDebug() << "强度图像显示完成：Y截面" << yIndex
        << "，图像尺寸：" << normalizedImage.cols << "x" << normalizedImage.rows;
}


/**
 * @brief 显示强度掩膜图像到页面
 * @param model 数据模型指针
 * @param view 用户界面视图指针
 * @param sliderValue 当前滑块值（Y截面索引）
 *
 * 该函数将强度掩膜数据转换为图像并在界面上显示
 */
void WindowDisplay::displayIntensityMaskImage(Model* model, OCTGUI* view, int sliderValue)
{
    // 安全检查：确保模型和掩膜数据有效
    if (model == nullptr || model->oct_intensity_mask == nullptr) {
        qDebug() << "错误：模型或强度掩膜数据未初始化";
        return;
    }

    if (model->X_pixel <= 0 || model->Z_pixel <= 0 || model->Y_pixel < 0) {
        qDebug() << "错误：图像尺寸参数无效";
        return;
    }

    // 验证滑块值在有效范围内
    int yIndex = sliderValue;
    if (yIndex < 0 || yIndex >= model->Y_pixel) {
        qDebug() << "警告：滑块值超出范围，使用默认值0";
        yIndex = 0;
    }

    // 计算当前Y截面在数据中的起始索引
    int sliceSize = model->X_pixel * model->Z_pixel;
    int startIndex = yIndex * sliceSize;

    // 安全检查数据索引
    if (startIndex + sliceSize > model->Y_pixel * sliceSize) {
        qDebug() << "错误：数据索引超出范围";
        return;
    }

    // 创建B扫描图像矩阵
    cv::Mat bscanImage(model->X_pixel, model->Z_pixel, CV_8UC1);

    // 复制掩膜数据到图像矩阵
    memcpy(bscanImage.ptr<uint8_t>(0),
        model->oct_intensity_mask + startIndex,
        sliceSize * sizeof(uint8_t));

    // 转置图像矩阵以适应显示方向
    cv::transpose(bscanImage, bscanImage);

    // 将OpenCV矩阵转换为Qt的QImage格式
    QImage qimg(bscanImage.data, bscanImage.cols, bscanImage.rows,
        bscanImage.step, QImage::Format_Grayscale8);

    // 使用界面上的掩膜显示控件
    QLabel* targetLabel = view->ui.figure_mask;

    // 更新界面上的图像显示
    QPixmap pixmap = QPixmap::fromImage(qimg);
    QSize labelSize = targetLabel->size();

    // 缩放图像以适应label大小
    QPixmap scaledPixmap = pixmap.scaled(labelSize,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation);

    targetLabel->setPixmap(scaledPixmap);
    targetLabel->setAlignment(Qt::AlignCenter);

    qDebug() << "强度掩膜图像显示完成：Y截面" << yIndex
        << "，图像尺寸：" << bscanImage.cols << "x" << bscanImage.rows;
}

